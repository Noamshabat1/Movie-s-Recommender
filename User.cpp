// don't change those include
#include "User.h"
#include "RecommendationSystem.h"
#include <map>

User::User (const std::string &name, const rank_map &ratings,
            std::shared_ptr<RecommendationSystem> recommendation_system) :
    _user_name (name),
    _ranks (ratings),
    _recommendation_s (recommendation_system)
{

}

std::string User::get_name () const
{
  return _user_name;
}

void User::add_movie_to_rs (const std::string &name, int year,
                            const std::vector<double> &features,
                            double rate)
{
  sp_movie movie = _recommendation_s->add_movie (name, year, features);
  _ranks[movie] = rate;
}

const rank_map &User::get_ranks () const
{
  return _ranks;
}

sp_movie User::get_recommendation_by_content () const
{
  return _recommendation_s->recommend_by_content (*this);
}

sp_movie User::get_recommendation_by_cf (int k) const
{
  return _recommendation_s->recommend_by_cf (*this, k);
}

double
User::get_prediction_score_for_movie (const std::string &name, int year, int k)
const
{
  return _recommendation_s->predict_movie_score
      (*this, _recommendation_s->get_movie (name, year), k);
}

std::ostream &operator<< (std::ostream &os, const User &user)
{
  os << "name: " << user.get_name () << "\n" << *(user._recommendation_s);
  return os;
}


