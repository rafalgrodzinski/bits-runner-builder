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
    static weak_ptr<llvm::Module> llvmModule;
    static weak_ptr<llvm::IRBuilder<>> builder;
    static function<llvm::Type *(shared_ptr<ValueType>, bool)> llvmTypeForValueType;
    static function<llvm::AllocaInst *(llvm::Type*, string)> buildAlloca;

    llvm::Type *type;
    shared_ptr<ValueType> valueType;

    function<llvm::Value *()> valueLambda;
    function<llvm::Value *()> pointerValueLambda;

public:
    WrappedValue();

    static void setup(
        weak_ptr<llvm::Module> llvmModule,
        weak_ptr<llvm::IRBuilder<>> builder,
        function<llvm::Type *(shared_ptr<ValueType>, bool)> llvmTypeForValueType,
        function<llvm::AllocaInst *(llvm::Type*, string)> buildAlloca
    );

    static shared_ptr<WrappedValue> wrappedValue(llvm::Value *value, shared_ptr<ValueType> valueType);
    static shared_ptr<WrappedValue> wrappedPointerValue(llvm::Value *pointerValue, llvm::Type *pointeeType, shared_ptr<ValueType> valueType);
    static shared_ptr<WrappedValue> wrappedUIntValue(llvm::Type *type, uint64_t value, shared_ptr<ValueType> valueType);
    static shared_ptr<WrappedValue> wrappedNone(llvm::Type *type, shared_ptr<ValueType> valueType);

    llvm::Value *getValue() const;
    llvm::Value *getPointerValue() const;

    llvm::Constant *getConstantValue() const;
    llvm::GlobalVariable *getGlobalValue() const;

    llvm::Type *getType() const;
    llvm::ArrayType *getArrayType() const;
    llvm::StructType *getStructType() const;
    shared_ptr<ValueType> getValueType() const;

    bool isArray() const;
    bool isPointer() const;
    bool isBlobStruct() const;
    bool isProtoStruct() const;
};

#endif