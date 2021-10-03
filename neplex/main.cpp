#include "main.hpp"

using namespace std;
using namespace cmdl;

int main(int argc, char **argv){

  string usage;

  try{

    CmdlParser cmdl(argc,argv);

    usage = setupCmdlParser(cmdl);

    cmdl.parse();

    if ( cmdl.checkShortoption('h') == true ){

      cout << cmdl.infoUsage();

      return 0;

    }

    ifstream input(cmdl.getMandatoryArgument("file").c_str());

    if ( input == false ){

      cerr << "could not open '" << cmdl.getMandatoryArgument("file") << "'" << endl;
      exit(1);

    }

    NwkAst * nwk;
    NwkParser parser(&input);

    nwk = parser.parse();

    if  ( cmdl.checkShortoption('d') )
      cout << "Data:" << endl << nwk->toString();

    Network network(nwk,cmdl);

    delete nwk;

    if ( cmdl.checkShortoption('i') )
      cout << network.info() << endl;

    double optimum = network.networksimplex();

    if ( cmdl.checkShortoption('i') )
      cout << "ITERATIONS: " << network.getIterations() << endl;
    
    cout << "optimum: " << Network::nonScientific(optimum) << endl;
    
    
  } catch (Exception<CmdlParser> &e){

    e.show();
    
    cout << usage << endl;

    return 1;

  } catch (ExceptionBase &e){

    e.show();

    return 1;

  }

  return 0;

}

string setupCmdlParser(CmdlParser & cmdl){

  cmdl.addMandatoryArgument("file","filename of a network-file (.nwk)");

  cmdl.addShortoption('h',"help");
  cmdl.shortsynonym('h') << '?';
  cmdl.shortsupervisor() << 'h';
  
  cmdl.addShortoption('i',"network-information");
  cmdl.addShortoption('e',"shows entering and leaving edges");
  cmdl.addShortoption('p',"processing-information");
  cmdl.addShortoption('d',"debugging-information");
  cmdl.addShortoption('c',"shows the new costs between calculations");
  cmdl.addShortoption('n',"don't use optimized algorithms");
  cmdl.addShortoption('u',"show number of updated nodes");
  cmdl.addShortoption('j',"show number of current iteration");

  return cmdl.usage();

}
