#include "milp_examples.hpp"

#include <sstream>
#include <numeric>

using namespace scip_wrapper;
using namespace milp_examples;

/* keksklauer4 17.03.2021
 *
 * A specific team matching problem (from a question on Reddit).
 * Each team that is formed has at least three members and at most 5.
 * In general, the goal is to form those teams while minimizing the cost
 * with cost corresponding to the "suffering" of each member.
 */

void milp_examples::team_matching_problem(const std::vector<fuint32_t> &teams,
      const std::vector<Member> &members,
      const PreferenceVec &preferences)
{
  MILPSolver solver{"TeamMatchingProblem", SolverSense::MINIMIZE};

  // for every team, there is binary variable whether it is formed
  // and two constraints enforcing capacity (minNbInTeam <= val <= maxNbInTeam if the team is formed)
  std::map<fuint32_t, fuint32_t> teamVars{};
  std::map<fuint32_t, std::pair<fuint32_t, fuint32_t>> teamCsts{};
  for (auto team : teams)
  {
    fuint32_t teamVar = solver.createBinaryVar(0.0); // not contributing to objective
    teamVars.insert(std::make_pair(team, teamVar));

    fuint32_t teamConstraint1 = solver.createLinearConstraint(0.0, 2.0);
    fuint32_t teamConstraint2 = solver.createLinearConstraintGeq(0.0);
    teamCsts.insert(std::make_pair(team, std::make_pair(teamConstraint1, teamConstraint2)));
    solver.addToCst(teamConstraint1, teamVar, -3.0);
    solver.addToCst(teamConstraint2, teamVar, 5.0);
  }

  // for every member there is a constraint that that member has to be in exactly one team
  std::map<fuint32_t, fuint32_t> memberCsts{};
  for (const auto &member : members)
  {
    fuint32_t memberConstraint = solver.createLinearConstraintEq(1.0);
    memberCsts.insert(std::make_pair(member.memberID, memberConstraint));
  }

  // for every member there is a binary variable for whether that member is in the specific team
  std::multimap<fuint32_t, std::pair<fuint32_t, fuint32_t>> memberInTeamVars{}; // key is pair of memberID; value is pair of <teamID, memberInTeamVariableIndex>
  for (const auto &preference : preferences)
  {
    fuint32_t prefVar = solver.createBinaryVar(preference.cost);
    // add to member constraint (each member exactly in one team)
    solver.addToCst(memberCsts[preference.memberID], prefVar);

    // add to team constraints
    auto constraintIndices = teamCsts[preference.teamID];
    solver.addToCst(constraintIndices.first, prefVar);
    solver.addToCst(constraintIndices.second, prefVar, -1.0);

    // add to map for retrieving for output
    memberInTeamVars.insert(std::make_pair(preference.memberID, std::make_pair(preference.teamID, prefVar)));
  }

  solver.solve();

  // Check solution
  std::cout << (solver.getSolution() != nullptr ? "Sucess!" : "An error occurred...") << std::endl;

  if (solver.getSolution() == nullptr) return;

  // Outputting results:
  for (auto team : teams)
  {
    bool isFormed = solver.getBinaryValue(teamVars[team]);
    std::cout << "Team " << team << " is "
      << (isFormed ? "" : "not ") << "formed." << std::endl;
  }

  std::cout << std::endl;

  std::map<fuint32_t, fuint32_t> amountTeamMembers{};
  for (const auto &member : members)
  {
    auto range = memberInTeamVars.equal_range(member.memberID);
    for (auto it = range.first; it != range.second; ++it)
    {
      if (solver.getBinaryValue(it->second.second))
      {
        std::cout << member.name << " is in team " << it->second.first << "." << std::endl;

        if (amountTeamMembers.find(it->second.first) == amountTeamMembers.end())
        {
          amountTeamMembers.insert(std::make_pair(it->second.first, 1));
        }
        else
        {
          amountTeamMembers[it->second.first]++;
        }
      }
    }
  }

  std::cout << std::endl;
  for (auto teamIt = amountTeamMembers.begin(); teamIt != amountTeamMembers.end(); ++teamIt)
  {
    std::cout << "Team " << teamIt->first << " has " << teamIt->second << " members." << std::endl;
  }
}

/*
 * Example that that team matching is in fact NP-hard (even strongly NP-hard)
 * The reduction works as follows:
 * For every element in numbers, we create a member.
 * Then, we want to generate all possible triplets of elements in the numbers vector.
 * From each of those triplets, we generate a possible team and add preferences for
 * the team members (the elements of the possible triplet) so that those members can
 * be part of that team. Note that since each member must be in *exactly* one team,
 * and each team has to have at least 3 members (and at most 3 since only three
 * preferences are added), we gain a three partition. If the instance of the team
 * matching problem is infeasible, then there is no 3 partition. If it is feasible,
 * the algorithm returns the teams that were formed and which element is in which team.
 *
 * Note that we neither need the maximum capacity nor costs as the latter is always 0.0
 * and the former can not be surpassed as each team has exactly 3 preferences.
 */

void reduction_from_3p(const std::vector<fuint32_t>& numbers)
{
  if (numbers.size() % 3 != 0) return;
  fuint32_t m = numbers.size() / 3;
  fuint32_t t = std::accumulate(numbers.begin(), numbers.end(), 0);
  if (t % m != 0) return;
  t /= m;

  std::stringstream str{};

  // for each element, create a member
  std::vector<Member> members{};
  members.reserve(numbers.size());
  fuint32_t index = 0;
  for (auto it = numbers.begin(); it != numbers.end(); ++it, ++index)
  {
    str << index << "(" << *it << ")";
    members.push_back(Member{index, str.str()});
    str.str(std::string()); // clear string stream
  }

  // create a team for each possible triplet
  std::vector<fuint32_t> teams{};
  PreferenceVec preferences{};
  fuint32_t teamIndex = 0;
  for (fuint32_t i = 0; i < numbers.size(); i++)
  {
    for (fuint32_t j = i + 1; j < numbers.size(); j++)
    {
      for (fuint32_t k = j + 1; k < numbers.size(); k++)
      {
        if (numbers.at(i) + numbers.at(j) + numbers.at(k) == t)
        { // possible triplet found; create team and preferences
          teams.push_back(teamIndex);
          preferences.push_back(MemberPreferences{i, teamIndex, 0.0});
          preferences.push_back(MemberPreferences{j, teamIndex, 0.0});
          preferences.push_back(MemberPreferences{k, teamIndex, 0.0});
          teamIndex++;
        }
      }
    }
  }

  // now solve problem (if infeasible, then no 3 partition exists otherwise teams are the solution to 3 partition)
  milp_examples::team_matching_problem(teams, members, preferences);
}

int main()
{
  std::vector<fuint32_t> teams
  { 0, 1, 2, 3, 4, 5, 6};
  std::vector<Member> members
  {
    Member{0, "Boo"},
    Member{1, "Ricardo"},
    Member{2, "Yamchu"},
    Member{3, "C16"},
    Member{4, "Goku"},
    Member{5, "Krillin"},
    Member{6, "Piccolo"},
    Member{7, "Gohan"},
    Member{8, "Freeza"},
    Member{9, "Cell"},
    Member{10, "Mr. Satan"},
    Member{11, "Nappa"}
  };

  PreferenceVec preferences
  {
    MemberPreferences {0, 1, 0.0},
    // MemberPreferences {0, 2, 3.0},
    // MemberPreferences {0, 3, 3.0},
    MemberPreferences {1, 1, 0.0},
    MemberPreferences {1, 5, 4.0},
    MemberPreferences {1, 6, 4.0},
    MemberPreferences {2, 1, 0.0},
    MemberPreferences {2, 4, 1.0},
    MemberPreferences {2, 5, 4.0},
    MemberPreferences {3, 2, 3.0},
    MemberPreferences {3, 4, 6.0},
    MemberPreferences {3, 6, 3.0},
    MemberPreferences {4, 3, 2.0},
    MemberPreferences {4, 4, 3.0},
    MemberPreferences {4, 5, 3.0},
    MemberPreferences {5, 1, 2.0},
    MemberPreferences {5, 2, 1.0},
    MemberPreferences {5, 6, 7.0},
    MemberPreferences {6, 1, 6.0},
    MemberPreferences {6, 2, 3.0},
    MemberPreferences {6, 6, 5.0},
    MemberPreferences {7, 1, 0.0},
    MemberPreferences {7, 3, 6.0},
    MemberPreferences {7, 4, 7.0},
    MemberPreferences {8, 4, 8.0},
    MemberPreferences {8, 5, 2.0},
    MemberPreferences {8, 6, 3.0},
    MemberPreferences {9, 1, 1.0},
    MemberPreferences {9, 2, 3.0},
    MemberPreferences {9, 4, 4.0},
    MemberPreferences {10, 1, 2.0},
    MemberPreferences {10, 2, 4.0},
    MemberPreferences {10, 5, 4.0},
    MemberPreferences {11, 4, 3.0},
    MemberPreferences {11, 5, 2.0},
    MemberPreferences {11, 6, 1.0}
  };

  milp_examples::team_matching_problem(teams, members, preferences);

  std::vector<fuint32_t> threePartitionInstance1{
    20, 23, 25, 30, 49, 45, 27, 30, 30, 40, 22, 19
  }; // example taken from wikipedia

  reduction_from_3p(threePartitionInstance1);


  std::vector<fuint32_t> threePartitionInstance2{
    1, 2, 5, 6, 7, 9
  }; // example taken from wikipedia

  reduction_from_3p(threePartitionInstance2);
}
