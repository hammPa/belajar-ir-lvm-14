#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>

#include <memory>
#include <vector>
#include <map>
#include <string>

llvm::LLVMContext context;
llvm::IRBuilder<> builder(context);
std::unique_ptr<llvm::Module> module;


llvm::Function *printFunc = nullptr;

std::map<std::string, llvm::Value*> NamedValues;

llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* func, const std::string& varName) {
    llvm::IRBuilder<> tmpB(&func->getEntryBlock(),
                            func->getEntryBlock().begin());
    
    return tmpB.CreateAlloca(builder.getInt32Ty(), nullptr, varName.c_str());
}














llvm::Function* getPrintf(llvm::Module *m) {
    if(printFunc) return printFunc;

    std::vector<llvm::Type*> printfArgs;
    printfArgs.push_back(builder.getInt8PtrTy());

    llvm::FunctionType *printfType = llvm::FunctionType::get(
        builder.getInt32Ty(), printfArgs, true
    );

    printFunc = llvm::Function::Create(
        printfType,
        llvm::Function::ExternalLinkage,
        "printf",
        m
    );

    printFunc->setCallingConv(llvm::CallingConv::C);
    return printFunc;
}

void genPrintString(llvm::Value *strVal){
    llvm::Function *printFunc = getPrintf(module.get());
    llvm::Value *fmt = builder.CreateGlobalStringPtr("%s\n");
    builder.CreateCall(printFunc, {fmt, strVal});
}

void genPrintInt(llvm::Value *intVal) {
    llvm::Function *printfFunc = getPrintf(module.get());
    llvm::Value *fmt = builder.CreateGlobalStringPtr("%d\n");
    builder.CreateCall(printfFunc, {fmt, intVal});
}

int main(){
    module = std::make_unique<llvm::Module>("my language", context);

    // buat fungsi main
    llvm::FunctionType *mainType = llvm::FunctionType::get(builder.getInt32Ty(), false);
    llvm::Function *mainFunc = llvm::Function::Create(
        mainType,
        llvm::Function::ExternalLinkage,
        "main",
        module.get()
    );
    llvm::BasicBlock *entry = llvm::BasicBlock::Create(context, "entry", mainFunc);
    builder.SetInsertPoint(entry);



    // print string
    llvm::Value *str = builder.CreateGlobalStringPtr("Saya baru belajar selama 1 bulan");
    genPrintString(str);

    // print angka 10 + 4 * 6 / 2
    llvm::Value *num = builder.getInt32(10);
    llvm::Value *mul = builder.CreateMul(builder.getInt32(4), builder.getInt32(6));
    llvm::Value *div = builder.CreateSDiv(mul, builder.getInt32(2));
    llvm::Value *sum = builder.CreateAdd(num, div);
    genPrintInt(sum);






    // var a = 10;
    llvm::AllocaInst* allocA = createEntryBlockAlloca(mainFunc, "a");
    builder.CreateStore(builder.getInt32(10), allocA);
    NamedValues["a"] = allocA;

    // load dan print
    llvm::Value *loadA = builder.CreateLoad(builder.getInt32Ty(), NamedValues["a"], "a");
    genPrintInt(loadA);




    // if(a >= 10)
    // llvm::Value* cond = builder.CreateICmpSGE(loadA, builder.getInt32(10), "cmp");

    // block then dan merge
    // llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(context, "then", mainFunc);
    // llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(context, "merge");
    

    // conditional branch
    // builder.CreateCondBr(cond, thenBB, mergeBB);

    // isi blok then
    // builder.SetInsertPoint(thenBB);
    // llvm::Value* text = builder.CreateGlobalStringPtr("angka 10\n");
    // genPrintString(text);
    // builder.CreateBr(mergeBB);

    // masukkan merge ke block function
    // mainFunc->getBasicBlockList().push_back(mergeBB);
    // builder.SetInsertPoint(mergeBB);





    // if(a <= 3){ printf("if"); }
    // elif(a <= 6) { printf("elif"); }
    // else { printf("else"); }

    llvm::BasicBlock *thenBB = llvm::BasicBlock::Create(context, "then", mainFunc);
    llvm::BasicBlock *elifBB = llvm::BasicBlock::Create(context, "elif", mainFunc);
    llvm::BasicBlock *thenBB2 = llvm::BasicBlock::Create(context, "then2", mainFunc);
    llvm::BasicBlock *elseBB = llvm::BasicBlock::Create(context, "else", mainFunc);
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(context, "merge", mainFunc);
    
    // kondisi if
    llvm::Value* condIf = builder.CreateICmpSLE(loadA, builder.getInt32(3), "ifcond");
    builder.CreateCondBr(condIf, thenBB, elifBB);

    // block then
    builder.SetInsertPoint(thenBB);
    llvm::Value *text = builder.CreateGlobalStringPtr("if block\n");
    genPrintString(text);
    builder.CreateBr(mergeBB);

    // kondisi elif
    builder.SetInsertPoint(elifBB);
    llvm::Value *condElif = builder.CreateICmpSLE(loadA, builder.getInt32(6), "elifcond");
    builder.CreateCondBr(condElif, thenBB2, elseBB);

    // then elif
    builder.SetInsertPoint(thenBB2);
    llvm::Value *text2 = builder.CreateGlobalStringPtr("elif block\n");
    genPrintString(text2);
    builder.CreateBr(mergeBB);

    // block else
    builder.SetInsertPoint(elseBB);
    llvm::Value *text3 = builder.CreateGlobalStringPtr("else block\n");
    genPrintString(text3);
    builder.CreateBr(mergeBB);

    // block merge
    builder.SetInsertPoint(mergeBB);







    // buat variabel i
    llvm::AllocaInst *allocI = createEntryBlockAlloca(mainFunc, "i");

    // i = 0
    builder.CreateStore(builder.getInt32(0), allocI);

    // basic block loop cond, loop body, loop inc, loop end
    llvm::BasicBlock *loopCondBB = llvm::BasicBlock::Create(context, "loop_cond", mainFunc);
    llvm::BasicBlock *loopBodyBB = llvm::BasicBlock::Create(context, "loop_body", mainFunc);
    llvm::BasicBlock *loopIncBB = llvm::BasicBlock::Create(context, "loop_inc", mainFunc);
    llvm::BasicBlock *loopEndBB = llvm::BasicBlock::Create(context, "loop_end", mainFunc);


    // branch dari entry ke loop cond
    builder.CreateBr(loopCondBB);

    // set insert point ke loop_cond
    builder.SetInsertPoint(loopCondBB);

    // load i
    llvm::Value *currentI = builder.CreateLoad(builder.getInt32Ty(), allocI, "i");

    llvm::Value *cond = builder.CreateICmpSLE(currentI, builder.getInt32(5 - 1), "loopcond");


    // branch kondisional ke body atau loop end
    builder.CreateCondBr(cond, loopBodyBB, loopEndBB);

    // isi loop_body
    builder.SetInsertPoint(loopBodyBB);

    // panggil print(i)
    genPrintInt(currentI);

    // lanjut ke loop_inc
    builder.CreateBr(loopIncBB);

    // isi loop_inc
    builder.SetInsertPoint(loopIncBB);

    // increment i
    llvm::Value *nextI = builder.CreateAdd(currentI, builder.getInt32(1), "nexti");
    builder.CreateStore(nextI, allocI);

    // balik lagi ke loop_cond
    builder.CreateBr(loopCondBB);

    // set insert point ke loop_end untuk instruksi setelah loop
    builder.SetInsertPoint(loopEndBB);


    builder.CreateRet(builder.getInt32(0));
    module->print(llvm::outs(), nullptr);






    std::error_code EC;
    llvm::raw_fd_ostream dest("output.ll", EC, llvm::sys::fs::OF_None);
    if (EC) {
        llvm::errs() << "Tidak bisa membuat file: " << EC.message() << "\n";
        return 1;
    }
    module->print(dest, nullptr);

    return 0;
}
