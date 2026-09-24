#ifndef WRAPPED_VALUE_H
#define WRAPPED_VALUE_H

#include <format>
#include <memory>
#include <string>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>

class ValueType;

class WrappedValue {
public:
    WrappedValue();

    static void setup(
        std::weak_ptr<llvm::Module> llvmModule,
        std::weak_ptr<llvm::IRBuilder<>> builder,
        std::function<llvm::Type *(std::shared_ptr<ValueType>, bool)> llvmTypeForValueType,
        std::function<llvm::AllocaInst *(llvm::Type*, std::string)> buildAlloca
    );

    static std::shared_ptr<WrappedValue> wrappedValue(llvm::Value *value, std::shared_ptr<ValueType> valueType);
    static std::shared_ptr<WrappedValue> wrappedPointerValue(llvm::Value *pointerValue, std::shared_ptr<ValueType> pointeeValueType);
    static std::shared_ptr<WrappedValue> wrappedUIntValue(llvm::Type *type, uint64_t value, std::shared_ptr<ValueType> valueType);
    static std::shared_ptr<WrappedValue> wrappedNone(llvm::Type *type, std::shared_ptr<ValueType> valueType);

    llvm::Value *getValue() const;
    llvm::Value *getPointerValue() const;

    llvm::Constant *getConstantValue() const;
    llvm::GlobalVariable *getGlobalValue() const;

    llvm::Type *getType() const;
    llvm::ArrayType *getArrayType() const;
    llvm::StructType *getStructType() const;
    std::shared_ptr<ValueType> getValueType() const;

    bool isArray() const;
    bool isPtr() const;
    bool isBlobStruct() const;
    bool isEnumStruct() const;
    bool isProtoStruct() const;

private:
    static std::weak_ptr<llvm::Module> llvmModule;
    static std::weak_ptr<llvm::IRBuilder<>> builder;
    static std::function<llvm::Type *(std::shared_ptr<ValueType>, bool)> llvmTypeForValueType;
    static std::function<llvm::AllocaInst *(llvm::Type*, std::string)> buildAlloca;

    llvm::Type *type;
    std::shared_ptr<ValueType> valueType;

    std::function<llvm::Value *()> valueLambda;
    std::function<llvm::Value *()> pointerValueLambda;
};

#endif