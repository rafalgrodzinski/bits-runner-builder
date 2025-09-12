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
#include <llvm/Support/NVPTXAddrSpace.h>

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
class ExpressionUnary;
enum class ExpressionBinaryOperation;

class Statement;
class StatementImport;
class StatementFunction;
class StatementFunctionDeclaration;
class StatementRawFunction;
class StatementBlob;
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
    map<string, llvm::StructType*> structTypeMap;
    map<string, vector<string>> structMembersMap;
} Scope;

class ModuleBuilder {
private:
    vector<shared_ptr<Error>> errors;
    string moduleName;
    string defaultModuleName;

    shared_ptr<llvm::LLVMContext> context;
    shared_ptr<llvm::Module> module;
    shared_ptr<llvm::IRBuilder<>> builder;

    llvm::Type *typeVoid;
    llvm::Type *typeBool;
    llvm::IntegerType *typeU8;
    llvm::IntegerType *typeU32;
    llvm::IntegerType *typeU64;
    llvm::IntegerType *typeS8;
    llvm::IntegerType *typeS32;
    llvm::IntegerType *typeS64;
    llvm::Type *typeR32;
    llvm::Type *typePtr;

    vector<shared_ptr<Statement>> statements;
    vector<shared_ptr<Statement>> headerStatements;
    map<string, vector<shared_ptr<Statement>>> exportedHeaderStatementsMap;
    stack<Scope> scopes;

    void buildStatement(shared_ptr<Statement> statement);
    void buildImportStatement(shared_ptr<Statement> statement, string moduleName);

    void buildImport(shared_ptr<StatementImport> statement);
    void buildFunction(shared_ptr<StatementFunction> statement);
    void buildRawFunction(shared_ptr<StatementRawFunction> statement);
    void buildBlob(shared_ptr<StatementBlob> statement);
    void buildVariable(shared_ptr<StatementVariable> statement);
    void buildAssignment(shared_ptr<StatementAssignment> statement);
    void buildBlock(shared_ptr<StatementBlock> statement);
    void buildReturn(shared_ptr<StatementReturn> statement);
    void buildRepeat(shared_ptr<StatementRepeat> statement);
    void buildExpression(shared_ptr<StatementExpression> statement);

    llvm::Value *valueForExpression(shared_ptr<Expression> expression, llvm::Type *castToType = nullptr);
    vector<llvm::Value*> valuesForExpression(shared_ptr<Expression> expression);
    llvm::Value *valueForLiteral(shared_ptr<ExpressionLiteral> expression, llvm::Type *castToType = nullptr);
    vector<llvm::Value*> valuesForArrayLiteral(shared_ptr<ExpressionArrayLiteral> expression);
    llvm::Value *valueForGrouping(shared_ptr<ExpressionGrouping> expression);
    llvm::Value *valueForBinary(shared_ptr<ExpressionBinary> expression);
    llvm::Value *valueForBinaryBool(ExpressionBinaryOperation operation, llvm::Value *leftValue, llvm::Value *rightValue);
    llvm::Value *valueForBinaryUnsignedInteger(ExpressionBinaryOperation operation, llvm::Value *leftValue, llvm::Value *rightValue);
    llvm::Value *valueForBinarySignedInteger(ExpressionBinaryOperation operation, llvm::Value *leftValue, llvm::Value *rightValue);
    llvm::Value *valueForBinaryReal(ExpressionBinaryOperation operation, llvm::Value *leftValue, llvm::Value *rightValue);
    llvm::Value *valueForUnary(shared_ptr<ExpressionUnary> expression);
    llvm::Value *valueForIfElse(shared_ptr<ExpressionIfElse> expression);
    llvm::Value *valueForVariable(shared_ptr<ExpressionVariable> expression);
    llvm::Value *valueForCall(shared_ptr<ExpressionCall> expression);
    llvm::Value *valueForArrayLiteral(shared_ptr<ExpressionArrayLiteral> expression);
    llvm::Value *valueForBuiltIn(llvm::AllocaInst *alloca, string memberName);

    void buildFunctionDeclaration(string moduleName, string name, bool isExtern, vector<pair<string, shared_ptr<ValueType>>> arguments, shared_ptr<ValueType> returnType);
    void buildAssignment(llvm::Value *targetValue, llvm::Type *targetType, shared_ptr<Expression> valueExpression);
    bool buildAssignmentForBuiltIn(llvm::AllocaInst *alloca, string memberName, shared_ptr<Expression> valueExpression);

    bool setAlloca(string name, llvm::AllocaInst *alloca);
    llvm::AllocaInst *getAlloca(string name);

    bool setFun(string name, llvm::Function *fun);
    llvm::Function *getFun(string name);

    bool setRawFun(string name, llvm::InlineAsm *rawFun);
    llvm::InlineAsm *getRawFun(string name);

    bool registerStruct(string structName, llvm::StructType *structType, vector<string> memberNames);
    llvm::StructType *getStructType(string structName);
    optional<int> getMemberIndex(string structName, string memberName);

    llvm::Type *typeForValueType(shared_ptr<ValueType> valueType, int count = 0);

    void markError(int line, int column, string message);

public:
    ModuleBuilder(string moduleName, string defaultModuleName, vector<shared_ptr<Statement>> statements, vector<shared_ptr<Statement>> headerStatements, map<string, vector<shared_ptr<Statement>>> exportedHeaderStatementsMap);
    shared_ptr<llvm::Module> getModule();
};

#endif