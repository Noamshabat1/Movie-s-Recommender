#include "UsersLoader.h"
#include "RecommendationSystemLoader.h"

int main()
{
  std::cout << "Creating system: " << std::endl;
  std::unique_ptr<RecommendationSystem> rs =
      RecommendationSystemLoader::create_rs_from_movies ("."
                                                           "./RecommenderSystemLoader_input.txt");

  std::cout << "Test 1: invalid value (>10)" << std::endl;
  try
  {
      RecommendationSystemLoader::create_rs_from_movies
        ("../RecommenderSystemLoader_input_no_good1.txt");
    std::cout << "didnt throw an exeption - Bad!" << std::endl;

  }
  catch (std::exception&)
  {
    std::cout << "throwed an exeption - good!" << std::endl;
  }

  std::cout << "Test 2: invalid value (<1)" << std::endl;
  try
  {
      RecommendationSystemLoader::create_rs_from_movies
        ("../RecommenderSystemLoader_input_no_good2.txt");
    std::cout << "didnt throw an exeption - Bad!" << std::endl;

  }
  catch (std::exception&)
  {
    std::cout << "throwed an exeption - good!" << std::endl;
  }
  std::cout << std::endl;
  std::cout << "Test: checking operator << on RecommendSystem"<< std::endl
  << "Should say: \n"
     "StarWars (1977)\n"
     "ForestGump (1994)\n"
     "Titanic (1997)\n"
     "Twilight (2008)\n"
     "Batman (2022)\n"
     "Result: \n" << std::endl;

  std::cout << *rs;
  std::cout << std::endl;
  std::cout << "Creating users: " << std::endl;
  std::vector<User> users;
//  std::shared_ptr<RecommenderSystem> new_rs = std::move (rs);
  users = UsersLoader::create_users ("../RSUsersLoader_input"
                                                 ".txt", std::move (rs));
  std::cout << std::endl;
  for (int i = 0; i < users.size(); ++i)
  {
    std::cout << users[i];
    for (auto  const& x : users[i].get_ranks())
    {
      std::cout << "   " << x.second << "    ";
    }
    std::cout << std::endl;
  }

  std::cout << std::endl;
  User sophia = users[0];
  User nicole = users[2];

  std::cout << "Checking get_recommendation_by_content() for all users:" <<
            std::endl << "Should say: Batman, Twilight, Titanic, Twilight (I"
                         " think...)" << std::endl;

  for(auto const &x : users)
  {
    std::cout << *x.get_recommendation_by_content() << std::endl;
  }

  //// checking get_prediction_score_for_movie()
  std::cout << " get_prediction_score_for_movie(): Should say 3.5244" <<
  std::endl << "Result : "<< nicole
      .get_prediction_score_for_movie("Twilight", 2008, 2) <<
            std::endl;

  //// checking get_recommendation_by_cf()
  std::cout << " get_recommendation_by_cf(): Should say Titanic" <<
  std::endl << "Result : " <<
  nicole
      .get_recommendation_by_cf(2)->get_name() <<
            std::endl;

  std::cout << " get_recommendation_by_cf(100): Should say Titanic" <<
  std::endl << "Result : " <<
            nicole
                .get_recommendation_by_cf(100)->get_name() <<
            std::endl;


  std::cout << "Test : adding no good movie to RecommendSystem" << std::endl;
  std::vector<double> new_vec = {1,2,-3,4};
  Movie new_movie("newMovie1",1929);
  try {
    sophia.add_movie_to_rs (new_movie.get_name(), new_movie.get_year(),
                            new_vec, 5);
    std::cout << "you added an invalid movie! - bad!" << std::endl;
    return 1;
  }
  catch (std::exception&)
  {
    std::cout << "you throwed an exception! - good!" << std::endl;
  }

  std::cout << "Test : adding movie to RecommendSystem" << std::endl <<
            "should print: \nname: Sofia\n"
            "newMovie (1930)\n"
            "StarWars (1977)\n"
            "ForestGump (1994)\n"
            "Titanic (1997)\n"
            "Twilight (2008)\n"
            "Batman (2022)\n"
            "\n"
            "Result: \n";
  std::vector<double> new_vec_2 = {1,2,3,4};
  Movie new_movie_2("newMovie",1930);
  sophia.add_movie_to_rs (new_movie_2.get_name(), new_movie_2.get_year(),
                          new_vec_2, 5);
  std::cout << sophia << std::endl;
  return 0;
}



