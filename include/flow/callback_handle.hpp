#pragma once

#include "flow/cancellation.hpp"
#include "flow/logging.hpp"

#include <sstream>

namespace flow {
enum class callback_type { publisher,
  subscription };

struct callback_info {
  std::size_t id;
  callback_type type;
  std::string channel_name;
  std::reference_wrapper<const std::type_info> message_type;
};

/**
 * A callback handle is a handle that has various controls up the communication heirarchy.
 *
 * It is given back when subscribing or publishing to a channel.
 */
template<typename config_t>
class callback_handle {
public:
  callback_handle() = default;
  callback_handle(callback_handle&&) = default;
  callback_handle(callback_handle const&) = default;
  callback_handle& operator=(callback_handle&&) = default;
  callback_handle& operator=(callback_handle const&) = default;

  callback_handle(callback_info&& info, cancellation_handle&& ch)
    : m_info(std::move(info)), m_cancel_handle(std::move(ch)) {}

  std::size_t id() const { return m_info.id; };
  callback_type type() const { return m_info.type; }
  std::string channel_name() const { return m_info.channel_name; }
  std::reference_wrapper<const std::type_info> message_info() const { return m_info.message_type; }

  void disable()
  {
    m_is_disabled = true;
    m_cancel_handle.request_cancellation();
  }

  bool is_disabled() const
  {
    return m_is_disabled;
  }


private:
  bool m_is_disabled{ false };
  callback_info m_info;
  cancellation_handle m_cancel_handle;
};

inline std::string to_string(callback_type type)
{
  // TODO: use reflection here
  switch (type) {
  case callback_type::publisher:
    return "publisher";
  case callback_type::subscription:
    return "subscription";
  default:
    flow::logging::critical_throw("Invalid callback type passed in to to_string.");
  }

  return "";
}

template<typename config_t>
std::string to_string(callback_handle<config_t> const& handle)
{
  std::stringstream ss;
  ss << "callback_handle: { ";
  const auto add_pair = [&ss](std::string_view item_name, auto&& item, std::string_view delim = ",") {
    ss << delim << " " << item_name << ": " << std::forward<decltype(item)>(item);
  };
  add_pair("id", handle.id(), "");
  add_pair("type", to_string(handle.type()));
  add_pair("channel_name", handle.channel_name());
  add_pair("message", handle.message_info().get().name());
  add_pair("is_disabled", handle.is_disabled() ? "true" : "false");
  ss << " }";

  return ss.str();
}
}// namespace flow