#include <main.hpp>  

using namespace std;
using namespace cmdl;

int main(int argc, char **argv) {

  string usage;

  CmdlParser cmdlparser(argc,argv);
  LP * lp;
  string lpath;    //path is for filepath, line corresponds to a line in given file
  pair<bool,string> res;

  try{

    usage = setupCmdlParser(cmdlparser);

    cmdlparser.parse();

    if ( cmdlparser.checkOption("help") == true || cmdlparser.checkShortoption('h') ){
      
      cout << cmdlparser.infoUsage() << endl;
      exit(0);
      
    }
    
    lpath = cmdlparser.getParameter("file");

    LPParser lpparser(lpath,cmdlparser);

    lp = lpparser.parse();

    lp->simplex();


    cout << endl;

    delete lp;

    return (0);


  } catch (Exception<CmdlParser> &e){
    
    e.show();
    cerr << usage << endl;
    
  } catch (ExceptionBase &e){

    e.show();

  }


}

string setupCmdlParser(CmdlParser &cmdlparser){

  cmdlparser.addParameter("file","filename","Name of an LP-file",true);
  cmdlparser.addOption("lexic","chooses lexicographic ordering for choosing the proper exiting-variable");
  cmdlparser.addOption("blands","chooses bland's rule for choosing the proper exiting-variable");
  cmdlparser.addOption("random","chooses random rule for choosing the proper exiting-variable");

  cmdlparser.addShortoption('h',"help");
  cmdlparser.addOption("help","help");

  cmdlparser.supervisor() << "help";
  cmdlparser.shortsupervisor() << 'h';

  return cmdlparser.usage();

}
