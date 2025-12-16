#include "WrappedValue.h"

WrappedValue::WrappedValue() { }

shared_ptr<WrappedValue> WrappedValue::wrappedValue(shared_ptr<llvm::IRBuilder<>> builder, llvm::Value *value) {
    shared_ptr<WrappedValue> wrappedValue = make_shared<WrappedValue>();

    //value->print(llvm::outs());
    //llvm::outs() << "\n";
    //value->getType()->print(llvm::outs());
    //llvm::outs() << "\n\n";

    if (llvm::LoadInst *loadInst = llvm::dyn_cast<llvm::LoadInst>(value)) {
        wrappedValue->value = value;
        wrappedValue->pointerValue = loadInst->getPointerOperand();
        wrappedValue->type = value->getType();
    } else if (llvm::AllocaInst *allocaInst = llvm::dyn_cast<llvm::AllocaInst>(value)) {
        wrappedValue->value = value;
        wrappedValue->pointerValue = value;
        wrappedValue->type = allocaInst->getAllocatedType();
    } else if (llvm::CallInst *callInst = llvm::dyn_cast<llvm::CallInst>(value)) {
        llvm::FunctionType *funType = callInst->getFunctionType();
        llvm::Type *retType = funType->getReturnType();
        if (retType->isVoidTy()) {
            wrappedValue->value = llvm::UndefValue::get(retType);
            wrappedValue->pointerValue = llvm::UndefValue::get(retType);
            wrappedValue->type = retType;
        } else {
            llvm::AllocaInst *alloca = builder->CreateAlloca(retType, nullptr);
            builder->CreateStore(callInst, alloca);

            wrappedValue->value = callInst;
            wrappedValue->pointerValue = alloca;
            wrappedValue->type = retType;
        }
    } else {
        wrappedValue->value = value;
        wrappedValue->pointerValue = value;
        wrappedValue->type = value->getType();
    }

    wrappedValue->constantValue = llvm::dyn_cast<llvm::Constant>(value);

    //wrappedValue->value->print(llvm::outs());
    //wrappedValue->value->getType()->print(llvm::outs());
    //wrappedValue->type->print(llvm::outs());

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