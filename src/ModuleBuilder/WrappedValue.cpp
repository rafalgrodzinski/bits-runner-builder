#include "WrappedValue.h"

WrappedValue::WrappedValue() { }

shared_ptr<WrappedValue> WrappedValue::wrappedValue(shared_ptr<llvm::IRBuilder<>> builder, llvm::Value *value, shared_ptr<ValueType> valueType) {
    shared_ptr<WrappedValue> wrappedValue = make_shared<WrappedValue>();

    wrappedValue->valueType = valueType;

    if (llvm::LoadInst *loadInst = llvm::dyn_cast<llvm::LoadInst>(value)) {
        wrappedValue->valueLambda = [loadInst](){ return loadInst; };
        wrappedValue->pointerValueLambda = [loadInst](){ return loadInst->getPointerOperand(); };

        wrappedValue->type = value->getType();
    } else if (llvm::AllocaInst *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(value)) {
        wrappedValue->valueLambda = [allocaInst](){ return allocaInst; };
        wrappedValue->pointerValueLambda = [allocaInst](){ return allocaInst; };

        wrappedValue->type = allocaInst->getAllocatedType();
    } else if (llvm::CallInst *callInst = llvm::dyn_cast<llvm::CallInst>(value)) {
        llvm::FunctionType *funType = callInst->getFunctionType();
        llvm::Type *retType = funType->getReturnType();
        if (retType->isVoidTy()) {
            wrappedValue->valueLambda = [retType](){ return llvm::UndefValue::get(retType); };
            wrappedValue->pointerValueLambda = [retType](){ return llvm::UndefValue::get(retType); };

            wrappedValue->type = retType;
        } else {
            wrappedValue->valueLambda = [callInst](){ return callInst; };
            wrappedValue->pointerValueLambda = [builder, retType, callInst](){
                llvm::AllocaInst *alloca = builder->CreateAlloca(retType, nullptr);
                builder->CreateStore(callInst, alloca);
                return alloca;
            };

            wrappedValue->type = retType;
        }
    } else if (llvm::Argument *argument = llvm::dyn_cast<llvm::Argument>(value)) {
        wrappedValue->valueLambda = [argument]() { return argument; };
        wrappedValue->pointerValueLambda = [argument](){ return argument; };

        wrappedValue->type = value->getType();
    } else {
        wrappedValue->valueLambda = [value]() { return value; };
        wrappedValue->pointerValueLambda = [value]() { return value; };

        wrappedValue->type = value->getType();
    }

    wrappedValue->constantValue = llvm::dyn_cast<llvm::Constant>(value);

    return wrappedValue;
}

shared_ptr<WrappedValue> WrappedValue::wrappedUIntValue(llvm::Type *type, int64_t value, shared_ptr<ValueType> valueType) {
    shared_ptr<WrappedValue> wrappedValue = make_shared<WrappedValue>();

    wrappedValue->valueLambda = [type, value]() { return llvm::ConstantInt::get(type, value, false); };
    wrappedValue->pointerValueLambda = [type, value]() { return llvm::ConstantInt::get(type, value, false); };

    wrappedValue->type = type;
    wrappedValue->valueType = valueType;

    return wrappedValue;
}

shared_ptr<WrappedValue> WrappedValue::wrappedSIntValue(llvm::Type *type, int64_t value, shared_ptr<ValueType> valueType) {
    shared_ptr<WrappedValue> wrappedValue = make_shared<WrappedValue>();

    wrappedValue->valueLambda = [type, value]() { return llvm::ConstantInt::get(type, value, false); };
    wrappedValue->pointerValueLambda = [type, value]() { return llvm::ConstantInt::get(type, value, false); };

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
    return constantValue;
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
    return type->isArrayTy();
}

bool WrappedValue::isPointer() {
    return type->isPointerTy();
}

bool WrappedValue::isStruct() {
    return type->isStructTy();
}