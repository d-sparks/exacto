#include "../src/cgame.cpp"
#include <boost/algorithm/string.hpp>
#include "../src/magics.cpp"
#include "../src/moves.cpp"
#include "assert.h"

using namespace std;

int test1() {
  map<string, pair<int, BB>> suites = {
      {"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
       {5, 4865609}},
      {"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 0",
       {4, 4085603}},
      {"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 0", {5, 674624}},
      {"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
       {4, 422333}},
      {"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
       {4, 2103487}},
      {"r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 "
       "10",
       {4, 3894594}}};

  for (auto suite : suites) {
    // read suite
    string position = suite.first;
    auto expected = suite.second;
    int depth = expected.first;
    BB count = expected.second;

    cout << "Running perft test for position `" + position + "`" << endl;
    cout << "Expected: " << count;

    // split fen into components
    vector<string> pVect;
    boost::split(pVect, position, boost::is_any_of(" "));

    // run test
    CGame game =
        CGame(pVect[0], pVect[1], pVect[2], pVect[3], pVect[4], pVect[5]);
    BB perftCount = perft(&game, depth) - perft(&game, depth - 1);
    cout << ", got: " << perftCount << endl << endl;
    ASSERT(perftCount == count, "wrong count");
  }

  return 1;
}

int main() {
  int t = 0;

  magics::populateRookTables();
  magics::populateBishopTables();
  masks::generateOpposite();
  masks::generateInterceding();

  t += test1();

  cout << endl;
  cout << t << " test(s) OK" << endl;
  return 0;
}
