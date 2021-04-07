#include <llvm/Support/CommandLine.h>
#include <clang/Tooling/CommonOptionsParser.h>

#include "actions/frontendaction.h"
#include "utils/utils.h"

#include <string>

using namespace std;
using namespace llvm;
using namespace clang;
using namespace clang::tooling;

static llvm::cl::OptionCategory ctCategory("clang-tool options");
int main(int argc, const char **argv)
{
    // In Clang 13, the line below will produce a compile-time error because the CommonOptionParser 
    // constructor is protected; the easiest way to deal with this is to modify it to public 
    // in the included clang/Tooling/CommonOptionsParser.h
    CommonOptionsParser optionsParser(argc, argv, ctCategory);

    for(auto &sourceFile : optionsParser.getSourcePathList())
    {
        if(utils::fileExists(sourceFile) == false)
        {
            llvm::errs() << "File: " << sourceFile << " does not exist!\n";
            return -1;
        }

        auto sourcetxt = utils::getSourceCode(sourceFile);
        auto compileCommands = optionsParser.getCompilations().getCompileCommands(getAbsolutePath(sourceFile));

        std::vector<std::string> compileArgs = utils::getCompileArgs(compileCommands);
        compileArgs.push_back("-I" + utils::getClangBuiltInIncludePath(argv[0]));
        for(auto &s : compileArgs)
            llvm::outs() << s << "\n";

        auto xfrontendAction = std::make_unique<XFrontendAction>();
        utils::customRunToolOnCodeWithArgs(move(xfrontendAction), sourcetxt, compileArgs, sourceFile);
    }

    return 0;
}
