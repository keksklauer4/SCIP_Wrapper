#include "milp_examples.hpp"

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
  SCIPSolver solver{"TeamMatchingProblem", SolverSense::MINIMIZE};

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
}
