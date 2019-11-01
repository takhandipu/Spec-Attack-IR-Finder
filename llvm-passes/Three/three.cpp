#include "llvm/Pass.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

#include <vector>
#include <set>
#include <queue>

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

using namespace llvm;

namespace {
struct Hello : public ModulePass {
  static char ID;
  Hello() : ModulePass(ID) {}

  bool runOnModule(Module &M) override {
    //errs() << "Three: \n";
    std::set<Value *> targets;
    std::vector<StoreInst *> sources;
    //errs().write_escaped(M.getName()) << '\n';
    for(auto fi = M.begin(); fi != M.end(); fi++)
    {
      //errs() << fi->getName() << "\n";
      for(auto bi = fi->begin(); bi != fi->end(); bi++)
      {
        for(auto ii = bi->begin(); ii != bi->end(); ii++)
	{
	  Instruction *Inst = &(*ii);
	  BranchInst *brInst = dyn_cast<BranchInst>(Inst);
	  if(brInst != NULL)
	  {
	    for(auto dep_bi: brInst->successors())
	    {
	      for(auto dep_ii=dep_bi->begin(); dep_ii !=dep_bi->end(); dep_ii++)
	      {
	        Instruction *tmp = &(*dep_ii);
		CallInst *callInst = dyn_cast<CallInst>(tmp);
		if(callInst != NULL)
		{
		  //callInst->print(errs());
		  //errs()<<"\n";
		  if(callInst->getCalledFunction() == NULL)
		  {
		    targets.insert(callInst->getCalledValue());
		    //auto *callee = callInst->getCalledValue();
		  }
		  //Value *val = dyn_cast<Value>(callee);
		  //callee->print(errs());errs()<<"\n";
		}
	      }
	    }
	  }
	  if(StoreInst *storeInst = dyn_cast<StoreInst>(Inst))
	  {
	    sources.push_back(storeInst);
	  }
	  /*CallInst *callInst = dyn_cast<CallInst>(Inst);
	  if(callInst != NULL)
	  {
	    Inst->print(errs());
	    errs()<<"\n";
	  }*/
	}
      }
    }
    std::queue<Value *> start_list;
    std::set<Value *> present;

    for(unsigned i =0; i< sources.size(); i++)
    {
      StoreInst *current = sources[i];
      //errs()<<"\n";
      //current->print(errs());
      //errs()<<":\n";
      Value *v = current->getPointerOperand();
      start_list.push(v);
      present.insert(v);
      //v->print(errs());
      //errs()<<"\n";

      /*for(auto *user: v->users())
      {
	(&(*user))->print(errs());
        errs()<<"\n";
        if(CallInst *test = dyn_cast<CallInst>(&(*user)))
	{
	  test->print(errs());
	  if(targets.find(test)!=targets.end())
	  {
	    current->print(errs());
	    errs()<<" ";
	    test->print(errs());
	    errs()<<"\n";
	  }
	}
      }*/
    }
    while(start_list.size()>0)
    {
      Value * current = start_list.front();
      start_list.pop();
      //errs()<<"\n";
      //current->print(errs());
      //errs()<<":\n";
      for(auto *user: current->users())
      {
        Value * tmp = dyn_cast<Value>(&(*user));
	//tmp->print(errs());
	if(present.find(tmp)==present.end())
	{
	  start_list.push(tmp);
	  present.insert(tmp);
	}
      }
    }
    //errs() <<"\n\n";
    std::set<Value *> found;
    for(auto *tmp: present)
    {
      Value * current=&(*tmp);
      //current->print(errs());
      //errs()<<"\n";
      //if(CallInst *test = dyn_cast<CallInst>(current))
      //{
      if(targets.find(current)!=targets.end())
      {
	found.insert(current);
      }
      //}
    }
    errs()<<found.size()<<"\n";
    for(auto *tmp: found)
    {
      Value * current = &(*tmp);
      current->print(errs());
      errs()<<"\n";
    }

    return false;
  }
}; // end of struct Hello
}  // end of anonymous namespace

char Hello::ID = 0;
static RegisterPass<Hello> X("hello", "Hello World Pass",
                             false /* Only looks at CFG */,
                             true /* Analysis Pass */);

static RegisterStandardPasses Y(
    PassManagerBuilder::EP_EarlyAsPossible,
    [](const PassManagerBuilder &Builder,
       legacy::PassManagerBase &PM) { PM.add(new Hello()); });
