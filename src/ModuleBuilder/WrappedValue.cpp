#include "WrappedValue.h"

#include "Parser/ValueType.h"

weak_ptr<llvm::Module> WrappedValue::llvmModule;
weak_ptr<llvm::IRBuilder<>> WrappedValue::builder;
function<llvm::Type *(shared_ptr<ValueType>, bool)> WrappedValue::llvmTypeForValueType;

WrappedValue::WrappedValue() { }

void WrappedValue::setup(weak_ptr<llvm::Module> llvmModule, weak_ptr<llvm::IRBuilder<>> builder, function<llvm::Type *(shared_ptr<ValueType>, bool)> llvmTypeForValueType) {
    WrappedValue::llvmModule = llvmModule;
    WrappedValue::builder = builder;
    WrappedValue::llvmTypeForValueType = llvmTypeForValueType;
}

shared_ptr<WrappedValue> WrappedValue::wrappedValue(llvm::Value *value, shared_ptr<ValueType> valueType) {
    shared_ptr<WrappedValue> wrappedValue = make_shared<WrappedValue>();

    llvm::Type *type = WrappedValue::llvmTypeForValueType(valueType, true);
    llvm::Type *allocaType = WrappedValue::llvmTypeForValueType(valueType, false);
    wrappedValue->valueType = valueType;
    wrappedValue->type = type;

    // Load
    if (llvm::LoadInst *loadInst = llvm::dyn_cast<llvm::LoadInst>(value)) {
        wrappedValue->valueLambda = [loadInst]() {
            return loadInst;
        };
        wrappedValue->pointerValueLambda = [loadInst]() {
            return loadInst->getPointerOperand();
        };
    // Alloca
    } else if (llvm::AllocaInst *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(value)) {
        wrappedValue->valueLambda = [type, allocaInst, valueType]() {
            llvm::LoadInst *load = WrappedValue::builder.lock()->CreateLoad(type, allocaInst, format("ld_wrp-{}", string(allocaInst->getName())));
            load->setVolatile(true);
            return load;
        };
        wrappedValue->pointerValueLambda = [allocaInst]() { 
            return allocaInst;
        };
    // Call
    } else if (llvm::CallInst *callInst = llvm::dyn_cast<llvm::CallInst>(value)) {
        if (type->isVoidTy()) {
            wrappedValue->valueLambda = [type]() {
                return llvm::UndefValue::get(type);
            };
            wrappedValue->pointerValueLambda = [type]() {
                return llvm::UndefValue::get(type);
            };
        } else {
            wrappedValue->valueLambda = [callInst]() {
                return callInst;
            };
            wrappedValue->pointerValueLambda = [allocaType, callInst]() {
                llvm::AllocaInst *alloca = WrappedValue::builder.lock()->CreateAlloca(allocaType, nullptr, "a_wrp");
                WrappedValue::builder.lock()->CreateStore(callInst, alloca);
                return alloca;
            };
        }
    // Function argument
    } else if (llvm::Argument *argument = llvm::dyn_cast<llvm::Argument>(value)) {
        wrappedValue->valueLambda = [argument]() {
            return argument;
        };
        wrappedValue->pointerValueLambda = [allocaType, argument]() {
            llvm::AllocaInst *alloca = WrappedValue::builder.lock()->CreateAlloca(allocaType, nullptr);
            WrappedValue::builder.lock()->CreateStore(argument, alloca);
            return alloca;
        };
    // Function
    } else if (llvm::Function *fun = llvm::dyn_cast<llvm::Function>(value)) {
        // it doesn't make sense to return a value to function
        wrappedValue->valueLambda = []() {
            return nullptr;
        };
        wrappedValue->pointerValueLambda = [fun]() {
            return fun;
        };
    // Global
    } else if (llvm::GlobalVariable *global = llvm::dyn_cast<llvm::GlobalVariable>(value)) {
        wrappedValue->valueLambda = [global]() {
            return WrappedValue::builder.lock()->CreateLoad(global->getValueType(), global, format("ld_wrp-{}", string(global->getName())));
        };
        wrappedValue->pointerValueLambda = [global]() {
            return global;
        };
    // Constant
    } else if (llvm::Constant *constant = llvm::dyn_cast<llvm::Constant>(value)) {
        if (value->getType()->isVoidTy()) {
            return WrappedValue::wrappedNone(value->getType(), valueType);
        }
        wrappedValue->valueLambda = [constant]() {
            return constant;
        };
        wrappedValue->pointerValueLambda = [constant]() {
            return new llvm::GlobalVariable(
                *WrappedValue::llvmModule.lock(),
                constant->getType(),
                true,
                llvm::GlobalValue::LinkageTypes::PrivateLinkage,
                constant
            );
        };
    // Value
    } else {
        wrappedValue->valueLambda = [value]() {
            return value;
        };
        wrappedValue->pointerValueLambda = [value]() {
            llvm::AllocaInst *allocaInst = WrappedValue::builder.lock()->CreateAlloca(value->getType(), nullptr, "a_wrp");
            WrappedValue::builder.lock()->CreateStore(value, allocaInst);
            return allocaInst;
        };
    }

    return wrappedValue;
}

shared_ptr<WrappedValue> WrappedValue::wrappedPointerValue(llvm::Value *pointerValue, llvm::Type *type, shared_ptr<ValueType> valueType) {
    shared_ptr<WrappedValue> wrappedValue = make_shared<WrappedValue>();

    llvm::Type *pointeeType = WrappedValue::llvmTypeForValueType(valueType, true);
    wrappedValue->type = pointeeType;
    wrappedValue->valueType = valueType;

    wrappedValue->valueLambda = [pointeeType, pointerValue, valueType]() {
        llvm::LoadInst *load = WrappedValue::builder.lock()->CreateLoad(pointeeType, pointerValue, format("ld_wrp-{}", string(pointerValue->getName())));
        load->setVolatile(true);
        return load;
    };
    wrappedValue->pointerValueLambda = [pointerValue]() {
        return pointerValue;
    };
    return wrappedValue;
}

shared_ptr<WrappedValue> WrappedValue::wrappedUIntValue(llvm::Type *type, uint64_t value, shared_ptr<ValueType> valueType) {
    shared_ptr<WrappedValue> wrappedValue = make_shared<WrappedValue>();

    wrappedValue->valueLambda = [type, value]() {
        return llvm::ConstantInt::get(type, value, false);
    };
    wrappedValue->pointerValueLambda = [type, value]() {
        return llvm::ConstantInt::get(type, value, false);
    };

    wrappedValue->type = type;
    wrappedValue->valueType = valueType;

    return wrappedValue;
}

shared_ptr<WrappedValue> WrappedValue::wrappedNone(llvm::Type *type, shared_ptr<ValueType> valueType) {
    shared_ptr<WrappedValue> wrappedValue = make_shared<WrappedValue>();

    wrappedValue->valueLambda = [type]() {
        return llvm::UndefValue::get(type);
    };
    wrappedValue->pointerValueLambda = [type]() {
        return llvm::UndefValue::get(type);
    };

    wrappedValue->type = type;
    wrappedValue->valueType = valueType;

    return wrappedValue;
}

llvm::Value *WrappedValue::getValue() {
    return valueLambda();
}

llvm::Value *WrappedValue::getPointerValue() {
    return pointerValueLambda();
}

llvm::Value *WrappedValue::getUnboxedValue() {
    llvm::Value *value = getValue();
    if (value == nullptr)
        return nullptr;
    llvm::Type *unboxedLlvmType = WrappedValue::llvmTypeForValueType(valueType, valueType->isBoxed());
    if (valueType->isBoxed()) {
        switch (valueType->getSubType()->getKind()) {
            case ValueTypeKind::BOOL:
            case ValueTypeKind::U8:
            case ValueTypeKind::U16:
            case ValueTypeKind::U32:
            case ValueTypeKind::S8:
            case ValueTypeKind::S16:
            case ValueTypeKind::S32:
                return WrappedValue::builder.lock()->CreateTruncOrBitCast(value, unboxedLlvmType);
            case ValueTypeKind::F32:
            case ValueTypeKind::F64:
                return WrappedValue::builder.lock()->CreateFPCast(value, unboxedLlvmType);
            case ValueTypeKind::A:
            case ValueTypeKind::PTR:
                return WrappedValue::builder.lock()->CreateBitOrPointerCast(value, unboxedLlvmType);
            default:
                return (llvm::Value*)nullptr;
        }
    }
    return value;
}

llvm::Constant *WrappedValue::getConstantValue() {
    return llvm::dyn_cast<llvm::Constant>(getValue());
}

llvm::GlobalVariable *WrappedValue::getGlobalValue() {
    return llvm::dyn_cast<llvm::GlobalVariable>(getPointerValue());
}

llvm::Type *WrappedValue::getType() {
    return type;
}

llvm::ArrayType *WrappedValue::getArrayType() {
    return llvm::dyn_cast<llvm::ArrayType>(type);
}

llvm::StructType *WrappedValue::getStructType() {
    return llvm::dyn_cast<llvm::StructType>(type);
}

shared_ptr<ValueType> WrappedValue::getValueType() {
    return valueType;
}

bool WrappedValue::isArray() {
    return valueType->isData();
}

bool WrappedValue::isPointer() {
    return valueType->isPointer() || valueType->getKind() == ValueTypeKind::A;
}

bool WrappedValue::isBlobStruct() {
    return valueType->isBlob();
}

bool WrappedValue::isProtoStruct() {
    return valueType->isProto();
}