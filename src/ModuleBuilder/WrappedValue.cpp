#include "WrappedValue.h"

#include "Parser/ValueType.h"

function<llvm::Type *(shared_ptr<ValueType>, bool)> WrappedValue::typeForValueType;

WrappedValue::WrappedValue() { }

shared_ptr<WrappedValue> WrappedValue::wrappedValue(shared_ptr<llvm::Module> module, shared_ptr<llvm::IRBuilder<>> builder, llvm::Value *value, shared_ptr<ValueType> valueType) {
    shared_ptr<WrappedValue> wrappedValue = make_shared<WrappedValue>();

    llvm::Type *type = WrappedValue::typeForValueType(valueType, true);
    llvm::Type *allocaType = WrappedValue::typeForValueType(valueType, false);
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
        wrappedValue->valueLambda = [builder, type, allocaInst, valueType]() {
            llvm::LoadInst *load = builder->CreateLoad(type, allocaInst, format("ld_wrp-{}", string(allocaInst->getName())));
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
            wrappedValue->pointerValueLambda = [builder, allocaType, callInst]() {
                llvm::AllocaInst *alloca = builder->CreateAlloca(allocaType, nullptr, "a_wrp");
                builder->CreateStore(callInst, alloca);
                return alloca;
            };
        }
    // Function argument
    } else if (llvm::Argument *argument = llvm::dyn_cast<llvm::Argument>(value)) {
        wrappedValue->valueLambda = [argument]() {
            return argument;
        };
        wrappedValue->pointerValueLambda = [builder, allocaType, argument]() {
            llvm::AllocaInst *alloca = builder->CreateAlloca(allocaType, nullptr);
            builder->CreateStore(argument, alloca);
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
        wrappedValue->valueLambda = [builder, global]() {
            return builder->CreateLoad(global->getValueType(), global, format("ld_wrp-{}", string(global->getName())));
        };
        wrappedValue->pointerValueLambda = [builder, global]() {
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
        wrappedValue->pointerValueLambda = [module, constant]() {
            return new llvm::GlobalVariable(
                *module,
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
        wrappedValue->pointerValueLambda = [builder, value]() {
            llvm::AllocaInst *allocaInst = builder->CreateAlloca(value->getType(), nullptr, "a_wrp");
            builder->CreateStore(value, allocaInst);
            return allocaInst;
        };
    }

    return wrappedValue;
}

shared_ptr<WrappedValue> WrappedValue::wrappedPointerValue(shared_ptr<llvm::IRBuilder<>> builder, llvm::Value *pointerValue, llvm::Type *type, shared_ptr<ValueType> valueType) {
    shared_ptr<WrappedValue> wrappedValue = make_shared<WrappedValue>();

    llvm::Type *pointeeType = WrappedValue::typeForValueType(valueType, true);
    wrappedValue->type = pointeeType;
    wrappedValue->valueType = valueType;

    wrappedValue->valueLambda = [builder, pointeeType, pointerValue, valueType]() {
        llvm::LoadInst *load = builder->CreateLoad(pointeeType, pointerValue, format("ld_wrp-{}", string(pointerValue->getName())));
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

llvm::Value *WrappedValue::getBitcastValue(shared_ptr<llvm::IRBuilder<>> builder, llvm::Type *targetType) {
    llvm::Value *sourceValue = getValue();

    if (!targetType->isIntOrPtrTy() && !targetType->isFloatingPointTy())
        return sourceValue;

    if (targetType->isFloatingPointTy()) {
        return builder->CreateBitCast(sourceValue, targetType);
    } if (targetType->isPointerTy() || sourceValue->getType()->getIntegerBitWidth() < targetType->getIntegerBitWidth()) {
        return builder->CreateZExtOrBitCast(sourceValue, targetType);
    } else if (sourceValue->getType()->getIntegerBitWidth() > targetType->getIntegerBitWidth()) {
        return builder->CreateTruncOrBitCast(sourceValue, targetType);
    }
   
   return sourceValue;
   //return builder->CreateLoad(targetType, getPointerValue());
}

llvm::Value *WrappedValue::getPointerValue() {
    return pointerValueLambda();
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