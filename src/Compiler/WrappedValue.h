#ifndef WRAPPED_VALUE_H
#define WRAPPED_VALUE_H

#include <memory>

#include <llvm/IR/IRBuilder.h>

using namespace std;

class WrappedValue {
private:
    llvm::Value *value;
    llvm::Value *pointerValue;
    llvm::Constant *constantValue;
    llvm::Type *type;

public:
    WrappedValue();

    static shared_ptr<WrappedValue> wrappedValue(llvm::Value *value);

    llvm::Value *getValue();
    llvm::Value *getPointerValue();
    llvm::Value *getConstantValue();

    llvm::Type *getType();
    llvm::StructType *getStructType();

    bool isStruct();
};

#endif