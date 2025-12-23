#ifndef WRAPPED_VALUE_H
#define WRAPPED_VALUE_H

#include <memory>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>

class ValueType;

using namespace std;

class WrappedValue {
private:
    llvm::Constant *constantValue;
    llvm::Type *type;
    shared_ptr<ValueType> valueType;

    function<llvm::Value *()> valueLambda;
    function<llvm::Value *()> pointerValueLambda;

public:
    WrappedValue();

    static shared_ptr<WrappedValue> wrappedValue(shared_ptr<llvm::IRBuilder<>> builder, llvm::Value *value, shared_ptr<ValueType> valueType);
    static shared_ptr<WrappedValue> wrappedUIntValue(llvm::Type *type, int64_t value, shared_ptr<ValueType> valueType);
    static shared_ptr<WrappedValue> wrappedSIntValue(llvm::Type *type, int64_t value, shared_ptr<ValueType> valueType);
    static shared_ptr<WrappedValue> wrappedNone(llvm::Type *type, shared_ptr<ValueType> valueType);

    llvm::Value *getValue();
    llvm::Value *getPointerValue();
    llvm::Constant *getConstantValue();

    llvm::Type *getType();
    llvm::ArrayType *getArrayType();
    llvm::StructType *getStructType();
    shared_ptr<ValueType> getValueType();

    bool isArray();
    bool isPointer();
    bool isStruct();
};

#endif