#include "WrappedValue.h"

#include "Parser/ValueType.h"

WrappedValue::WrappedValue() { }

shared_ptr<WrappedValue> WrappedValue::wrappedValue(shared_ptr<llvm::Module> module, shared_ptr<llvm::IRBuilder<>> builder, llvm::Type *type, llvm::Value *value, shared_ptr<ValueType> valueType) {
    shared_ptr<WrappedValue> wrappedValue = make_shared<WrappedValue>();

    wrappedValue->type = type;
    wrappedValue->valueType = valueType;

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
        wrappedValue->valueLambda = [builder, allocaInst]() {
            return builder->CreateLoad(allocaInst->getAllocatedType(), allocaInst, format("ld_wrp-{}", string(allocaInst->getName())));
        };
        wrappedValue->pointerValueLambda = [allocaInst]() { 
            return allocaInst;
        };
    // Call
    } else if (llvm::CallInst *callInst = llvm::dyn_cast<llvm::CallInst>(value)) {
        llvm::FunctionType *funType = callInst->getFunctionType();
        llvm::Type *retType = funType->getReturnType();
        if (retType->isVoidTy()) {
            wrappedValue->valueLambda = [retType]() {
                return llvm::UndefValue::get(retType);
            };
            wrappedValue->pointerValueLambda = [retType]() {
                return llvm::UndefValue::get(retType);
            };
        } else {
            wrappedValue->valueLambda = [callInst]() {
                return callInst;
            };
            wrappedValue->pointerValueLambda = [builder, retType, callInst]() {
                llvm::AllocaInst *alloca = builder->CreateAlloca(retType, nullptr, "a_wrp");
                builder->CreateStore(callInst, alloca);
                return alloca;
            };
        }
    // Function argument
    } else if (llvm::Argument *argument = llvm::dyn_cast<llvm::Argument>(value)) {
        llvm::Type *type = value->getType();
        wrappedValue->valueLambda = [argument]() {
            return argument;
        };
        wrappedValue->pointerValueLambda = [builder, type, argument]() {
                llvm::AllocaInst *alloca = builder->CreateAlloca(type, nullptr);
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

shared_ptr<WrappedValue> WrappedValue::wrappedPointerValue(shared_ptr<llvm::Module> module, shared_ptr<llvm::IRBuilder<>> builder, llvm::Type *pointeeType, llvm::Value *pointerValue, shared_ptr<ValueType> valueType) {
    shared_ptr<WrappedValue> wrappedValue = make_shared<WrappedValue>();

    wrappedValue->type = pointeeType;
    wrappedValue->valueType = valueType;

    wrappedValue->valueLambda = [builder, pointeeType, pointerValue]() {
        return builder->CreateLoad(pointeeType, pointerValue, format("ld_wrp-{}", string(pointerValue->getName())));
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