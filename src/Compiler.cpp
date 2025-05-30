#include "Compiler.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Constants.h"
#include "llvm/Support/raw_ostream.h"

Compiler::Compiler(shared_ptr<Expression> expression): expression(expression) {
    context = make_shared<llvm::LLVMContext>();
    module = make_shared<llvm::Module>("dummy", *context);
    builder = make_shared<llvm::IRBuilder<>>(*context);

    //int32Type = shared_ptr<llvm::IntegerType>(llvm::Type::getInt32Ty(context));
    int32Type = llvm::Type::getInt32Ty(*context);
}

llvm::Value *Compiler::valueForExpression(shared_ptr<Expression> expression) {
    switch (expression->getKind()) {
        case Expression::Kind::LITERAL:
            return llvm::ConstantInt::get(int32Type, expression->getInteger(), true);
        case Expression::Kind::BINARY:
            llvm::Value *leftValue = valueForExpression(expression->getLeft());
            llvm::Value *rightValue = valueForExpression(expression->getRight());
            switch (expression->getOperator()) {
                case Expression::Operator::ADD:
                    return builder->CreateNSWAdd(leftValue, rightValue);
                case Expression::Operator::SUB:
                    return builder->CreateNSWSub(leftValue, rightValue);
                case Expression::Operator::MUL:
                    return builder->CreateNSWMul(leftValue, rightValue);
                case Expression::Operator::DIV:
                    return builder->CreateSDiv(leftValue, rightValue);
                case Expression::Operator::MOD:
                    return builder->CreateSRem(leftValue, rightValue);
            }
            break;
    }
}

shared_ptr<llvm::Module> Compiler::getModule() {
    llvm::Value *value = valueForExpression(expression);
    value->print(llvm::outs(), false);
    return module;
}