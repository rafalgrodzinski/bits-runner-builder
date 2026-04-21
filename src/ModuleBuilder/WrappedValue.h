#ifndef WRAPPED_VALUE_H
#define WRAPPED_VALUE_H

#include <format>
#include <memory>
#include <string>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>

class ValueType;

using namespace std;

class WrappedValue {
private:
    llvm::Type *type;
    shared_ptr<ValueType> valueType;

    function<llvm::Value *()> valueLambda;
    function<llvm::Value *()> unboxedValueLambda;
    function<llvm::Value *()> pointerValueLambda;
    function<llvm::Value *()> unboxedPointerValueLambda;

public:
    static function<llvm::Type *(shared_ptr<ValueType>, bool)> typeForValueType;

    WrappedValue();

    static shared_ptr<WrappedValue> wrappedValue(shared_ptr<llvm::Module> module, shared_ptr<llvm::IRBuilder<>> builder, llvm::Value *value, shared_ptr<ValueType> valueType);
    static shared_ptr<WrappedValue> wrappedPointerValue(shared_ptr<llvm::IRBuilder<>> builder, llvm::Value *pointerValue, llvm::Type *pointeeType, shared_ptr<ValueType> valueType);
    static shared_ptr<WrappedValue> wrappedUIntValue(llvm::Type *type, uint64_t value, shared_ptr<ValueType> valueType);
    static shared_ptr<WrappedValue> wrappedNone(llvm::Type *type, shared_ptr<ValueType> valueType);

    llvm::Value *getValue();
    llvm::Value *getUnboxedValue();

    llvm::Value *getPointerValue();
    llvm::Value *getUnboxedPointerValue();

    llvm::Value *getBitcastValue(shared_ptr<llvm::IRBuilder<>> builder, llvm::Type *targetType);

    llvm::Constant *getConstantValue();
    llvm::GlobalVariable *getGlobalValue();

    llvm::Type *getType();
    llvm::ArrayType *getArrayType();
    llvm::StructType *getStructType();
    shared_ptr<ValueType> getValueType();

    bool isArray();
    bool isPointer();
    bool isBlobStruct();
    bool isProtoStruct();
};

#endif