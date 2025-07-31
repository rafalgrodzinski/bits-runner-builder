#ifndef MODULE_BUILDER_H
#define MODULE_BUILDER_H

#include <map>
#include <stack>

#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/InlineAsm.h>

class Error;
class ValueType;

class Expression;
class ExpressionGrouping;
class ExpressionLiteral;
class ExpressionArrayLiteral;
class ExpressionVariable;
class ExpressionCall;
class ExpressionIfElse;
class ExpressionBinary;
enum class ExpressionBinaryOperation;

class Statement;
class StatementFunction;
class StatementRawFunction;
class StatementVariable;
class StatementAssignment;
class StatementReturn;
class StatementExpression;
class StatementRepeat;
class StatementMetaExternFunction;
class StatementBlock;

using namespace std;

typedef struct {
    map<string, llvm::AllocaInst*> allocaMap;
    map<string, llvm::Function*> funMap;
    map<string, llvm::InlineAsm*> rawFunMap;
} Scope;

class ModuleBuilder {
private:
    vector<shared_ptr<Error>> errors;
    string moduleName;
    string sourceFileName;

    shared_ptr<llvm::LLVMContext> context;
    shared_ptr<llvm::Module> module;
    shared_ptr<llvm::IRBuilder<>> builder;

    llvm::Type *typeVoid;
    llvm::Type *typeBool;
    llvm::IntegerType *typeU8;
    llvm::IntegerType *typeU32;
    llvm::IntegerType *typeS8;
    llvm::IntegerType *typeS32;
    llvm::Type *typeR32;

    vector<shared_ptr<Statement>> statements;
    stack<Scope> scopes;

    void buildStatement(shared_ptr<Statement> statement);
    void buildFunction(shared_ptr<StatementFunction> statement);
    void buildRawFunction(shared_ptr<StatementRawFunction> statement);
    void buildVarDeclaration(shared_ptr<StatementVariable> statement);
    void buildAssignment(shared_ptr<StatementAssignment> statement);
    void buildBlock(shared_ptr<StatementBlock> statement);
    void buildReturn(shared_ptr<StatementReturn> statement);
    void buildLoop(shared_ptr<StatementRepeat> statement);
    void buildMetaExternFunction(shared_ptr<StatementMetaExternFunction> statement);
    void buildExpression(shared_ptr<StatementExpression> statement);

    llvm::Value *valueForExpression(shared_ptr<Expression> expression);
    vector<llvm::Value*> valuesForExpression(shared_ptr<Expression> expression);
    llvm::Value *valueForLiteral(shared_ptr<ExpressionLiteral> expression);
    vector<llvm::Value*> valuesForArrayLiteral(shared_ptr<ExpressionArrayLiteral> expression);
    llvm::Value *valueForGrouping(shared_ptr<ExpressionGrouping> expression);
    llvm::Value *valueForBinary(shared_ptr<ExpressionBinary> expression);
    llvm::Value *valueForBinaryBool(ExpressionBinaryOperation operation, llvm::Value *leftValue, llvm::Value *rightValue);
    llvm::Value *valueForBinaryUnsignedInteger(ExpressionBinaryOperation operation, llvm::Value *leftValue, llvm::Value *rightValue);
    llvm::Value *valueForBinarySignedInteger(ExpressionBinaryOperation operation, llvm::Value *leftValue, llvm::Value *rightValue);
    llvm::Value *valueForBinaryReal(ExpressionBinaryOperation operation, llvm::Value *leftValue, llvm::Value *rightValue);
    llvm::Value *valueForIfElse(shared_ptr<ExpressionIfElse> expression);
    llvm::Value *valueForVar(shared_ptr<ExpressionVariable> expression);
    llvm::Value *valueForCall(shared_ptr<ExpressionCall> expression);

    bool setAlloca(string name, llvm::AllocaInst *alloca);
    llvm::AllocaInst *getAlloca(string name);

    bool setFun(string name, llvm::Function *fun);
    llvm::Function *getFun(string name);

    bool setRawFun(string name, llvm::InlineAsm *rawFun);
    llvm::InlineAsm *getRawFun(string name);

    llvm::Type *typeForValueType(shared_ptr<ValueType> valueType, int count = 0);

    void markError(int line, int column, string message);

public:
    ModuleBuilder(string moduleName, string sourceFileName, vector<shared_ptr<Statement>> statements);
    shared_ptr<llvm::Module> getModule();
};

#endif