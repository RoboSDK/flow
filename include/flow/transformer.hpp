#pragma once

#include "flow/concepts.hpp"

#include "flow/detail/cancellable_function.hpp"

namespace flow {
namespace detail {
  template<typename T>
  class transformer_impl;

  template<typename return_t, typename... args_t>
  class transformer_impl<return_t(args_t...)>;
}

/**
 * May be called directly instead of make_routine<flow::consumer>(args);
 *
 * These objects created are passed in to the network to spin up the routines
 *
 * @tparam argument_t The consumer tag
 * @param callback A consumer function
 * @param publish_to The channel to publish to
 * @param subscribe_to The channel to subscribe to
 * @return A consumer object used to retrieve data by the network
 */
template<typename return_t, typename argument_t>
auto transformer(std::function<return_t(argument_t&&)>&& callback, std::string subscribe_to, std::string publish_to)
{
  using callback_t = decltype(callback);
  return detail::transformer_impl<return_t(argument_t)>(std::forward<callback_t>(callback), std::move(subscribe_to), std::move(publish_to));
}

template<typename return_t, typename argument_t>
auto transformer(return_t (*callback)(argument_t&&), std::string subscribe_to, std::string publish_to)
{
  using callback_t = decltype(callback);
  return detail::transformer_impl<return_t(argument_t)>(std::forward<callback_t>(callback), std::move(subscribe_to), std::move(publish_to));
}

auto transformer(auto&& lambda, std::string subscribe_to, std::string publish_to)
{
  using callback_t = decltype(lambda);
  return transformer(detail::metaprogramming::to_function(std::forward<callback_t>(lambda)), std::move(subscribe_to), std::move(publish_to));
}

namespace detail {
template<typename return_t, typename... args_t>
class transformer_impl<return_t(args_t...)> {
public:
  using is_transformer = std::true_type;
  using is_routine = std::true_type;

  transformer_impl() = default;
  ~transformer_impl() = default;

  transformer_impl(transformer_impl&&) noexcept = default;
  transformer_impl(transformer_impl const&) = default;
  transformer_impl& operator=(transformer_impl&&) noexcept = default;
  transformer_impl& operator=(transformer_impl const&) = default;

  transformer_impl(flow::is_transformer_function auto&& callback, std::string producer_channel_name, std::string consumer_channel_name)
    : m_callback(detail::make_shared_cancellable_function(std::forward<decltype(callback)>(callback))),
      m_producer_channel_name(std::move(producer_channel_name)),
      m_consumer_channel_name(std::move(consumer_channel_name))
  {
  }

  auto subscribe_to() { return m_producer_channel_name; }
  auto publish_to() { return m_consumer_channel_name; }

  auto& callback() { return *m_callback; }

private:
  using callback_ptr = typename detail::cancellable_function<return_t(args_t&&...)>::sPtr;

  callback_ptr m_callback{ nullptr };
  std::string m_producer_channel_name{};
  std::string m_consumer_channel_name{};
};
}// namespace detail
}// namespace flow