#include <algorithm>
#include "RecommendationSystem.h"

bool movie_comparer_2 (std::pair<sp_movie, double> &v1,
                       std::pair<sp_movie, double> &v2)
{
  return (v2.second) < (v1.second);
}

sp_movie RecommendationSystem::add_movie (const std::string &name, int year,
                                          const std::vector<double> &features)
{
  sp_movie movie = std::make_shared<Movie> (name, year);
  _movies[movie] = std::vector<double> (features);
  return movie;
}

sp_movie RecommendationSystem::recommend_by_content (const User &user)
{
  /// ================== STEP 1 ==================
  rank_map ranks = user.get_ranks ();
  if (ranks.empty ())
    { return nullptr; }
  double sum = 0;
  for (const auto &p: ranks)
    { sum += p.second; }
  double avg = sum / ranks.size ();
  for (auto &p: ranks)
    { p.second -= avg; }
  /// ================== STEP 2 ==================
  unsigned long size = 0;
  if (!_movies.empty ())
    { size = _movies.begin ()->second.size (); }
  std::vector<double> pref_vec (size, 0);

  for (const auto &p: ranks)
    {
      const auto &movie = _movies.find (p.first);
      if (movie != _movies.end ())
        {
          const std::vector<double> &movie_features = movie->second;
          for (unsigned int k = 0; k < movie_features.size (); k++)
            { pref_vec[k] += p.second * movie_features[k]; }
        }
    }
  /// ================== STEP 3 ==================
  sp_movie best_movie = nullptr;
  double best_score = RANK_NA;
  for (const auto &entry: _movies)
    {
      if (ranks.find (entry.first) == ranks.end ())
        {
          const std::vector<double> &movie_features = entry.second;
          double movie_score = calculate_vector_expression
              (pref_vec, movie_features);

          if (movie_score > best_score || best_movie == nullptr)
            {
              best_movie = entry.first;
              best_score = movie_score;
            }
        }
    }
  return best_movie;
}

sp_movie RecommendationSystem::recommend_by_cf (const User &user, int k)
{
  rank_map prediction_ranks = rank_map (0, sp_movie_hash, sp_movie_equal);

  for (const auto &entry: _movies)
    {
      if (user.get_ranks ().find (entry.first) == user.get_ranks ().end ())
        {
          prediction_ranks[entry.first] = predict_movie_score (user, entry
              .first, k);
        }
    }
  double best_rank = RANK_NA;
  sp_movie best_movie;
  for (const auto &entry: prediction_ranks)
    {
      if (entry.second > best_rank || best_rank == RANK_NA)
        {
          best_movie = entry.first;
          best_rank = entry.second;
        }
    }
  return best_movie;
}

double
RecommendationSystem::predict_movie_score (const User &user, const
sp_movie &movie, int k)
{
  rank_map ranks = user.get_ranks ();
  if (ranks.empty ())
    { return 0; }

  std::vector<double> movie_data = _movies[movie];
  std::vector<std::pair<sp_movie, double>> similar_ranks;
  for (const auto &entry: ranks)
    {
      const std::vector<double> &entry_features = _movies[entry.first];
      double similarity_vals = calculate_vector_expression (movie_data,
                                                            entry_features);
      similar_ranks.emplace_back (entry.first, similarity_vals);
    }
  std::sort (similar_ranks.begin (), similar_ranks.end (), movie_comparer_2);
  double top = 0;
  double bottom = 0;
  for (int i = 0; i < k; i++)
    {
      const sp_movie &similar_movie = similar_ranks[i].first;
      double similarity = similar_ranks[i].second;

      if (ranks.find (similar_movie) != ranks.end ())
        {
          top += ranks[similar_movie] * similarity;
          bottom += similarity;
        }
    }
  if (bottom == 0)
    { return 0; }
  return top / bottom;
}

sp_movie
RecommendationSystem::get_movie (const std::string &name, int year) const
{
  sp_movie movie = std::make_shared<Movie> (name, year);
  auto entry = _movies.find (movie);

  if (entry != _movies.end ())
    {
      return entry->first;
    }
  return {nullptr};
}

std::ostream &operator<< (std::ostream &os, const RecommendationSystem &rs)
{
  for (const auto &entry: rs._movies)
    {
      os << (*entry.first);
    }
  os << std::flush << std::endl;
  return os;
}

//###########################################################################

double
RecommendationSystem::calculate_vector_expression (const std::vector<double>
&v1, const std::vector<double> &v2)
{
  double sum_of_element_wise = 0;
  double v1_size = 0;
  double v2_size = 0;

  for (size_t i = 0; i < v1.size (); i++)
    {
      sum_of_element_wise += v1[i] * v2[i];
      v1_size += v1[i] * v1[i];
      v2_size += v2[i] * v2[i];
    }
  double top = sum_of_element_wise;
  double bottom = (std::sqrt (v1_size) * std::sqrt (v2_size));
  double ans = top / bottom;
  return ans;
}

