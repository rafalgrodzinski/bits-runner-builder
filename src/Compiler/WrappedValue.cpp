#include "WrappedValue.h"

WrappedValue::WrappedValue() { }

shared_ptr<WrappedValue> WrappedValue::wrappedValue(llvm::Value *value) {
    shared_ptr<WrappedValue> wrappedValue = make_shared<WrappedValue>();

    value->print(llvm::outs());
    llvm::outs() << "\n";
    value->getType()->print(llvm::outs());
    llvm::outs() << "\n\n";

    if (llvm::LoadInst *loadInst = llvm::dyn_cast<llvm::LoadInst>(value)) {
        wrappedValue->value = value;
        wrappedValue->pointerValue = loadInst->getPointerOperand();
        wrappedValue->type = value->getType();
    } else if (llvm::AllocaInst *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(value)) {
        wrappedValue->value = value;
        wrappedValue->pointerValue = value;
        wrappedValue->type = allocaInst->getAllocatedType();
    } else if (llvm::CallInst *callInst = llvm::dyn_cast<llvm::CallInst>(value)) {
        //llvm::AllocaInst *alloca = builder->CreateAlloca(sourceType, nullptr);
        //builder->CreateStore(sourceValue, alloca);
        //sourceValue = alloca;
    } else {
        wrappedValue->value = value;
        wrappedValue->pointerValue = value;
        wrappedValue->type = value->getType();
    }

    wrappedValue->constantValue = llvm::dyn_cast<llvm::Constant>(value);

    return wrappedValue;
}

llvm::Value *WrappedValue::getValue() {
    return value;
}

llvm::Value *WrappedValue::getPointerValue() {
    return pointerValue;
}

llvm::Value *WrappedValue::getConstantValue() {
    return constantValue;
}

llvm::Type *WrappedValue::getType() {
    return type;
}

llvm::StructType *WrappedValue::getStructType() {
    return llvm::dyn_cast<llvm::StructType>(type);
}

bool WrappedValue::isStruct() {
    return type->isStructTy();
}