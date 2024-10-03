#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <string>
#include <tuple>
#include <utility>
#include <fixed_string.hpp>
#include <ctre.hpp>
#include <scn/scan.h>

namespace ox::commander {
// Базовый интерфейс команды

template<class... ARGS>
class ICommand
{
public:
	using args_t = std::tuple<ARGS...>;

private:
	static inline std::string m_prefix = "";
	thread_local static inline  args_t m_context

protected:
	ICommand ( ) = default;

public:
	static auto prefix ( ) -> std::string_view
	{
		return m_prefix;
	}

	static void prefix ( const std::string& prefix )
	{
		m_prefix = prefix;
	}

	static void 

	virtual ~ICommand ( ) = default;
	

	// Виртуальная функция для выполнения команды
	virtual void execute ( ) = 0;

	// Виртуальная функция для парсинга
	virtual bool parse ( std::string&& cmd ) = 0;

	virtual constexpr std::string_view name ( )  = 0;
	virtual constexpr std::string_view usage ( ) = 0;
};

// Шаблонный класс команды
template< fixstr::fixed_string Usage, class... Args > class CCommand : public ICommand
{
public:
	using args_t = std::tuple< Args... >;

private:
	thread_local inline static args_t m_args;

protected:
	args_t& args ( )
	{
		return m_args;
	}

public:
	~CCommand ( ) override = default;

	// Шаблонный метод для парсинга, если Usage не пуст
	bool parse ( std::string&& cmd ) override
	{
		if constexpr ( Usage.size ( ) != 0 && sizeof...( Args ) != 0 ) {
			if ( auto result = scn::scan< Args... > ( cmd, Usage ) ) {
				m_args = std::move ( result->value ( ) );
				LOGD << "Parsed: " << fmt::format ( "{}", m_args );
				return true;
			}
			LOGE << "Failed to parse command: " << cmd;
			return false;
		} else {
			return true;
		}
	}

	constexpr std::string_view name ( ) override
	{
		{
			constexpr auto pattern = ctll::fixed_string { R"(^(\w+).*)" }; // example: <prefix><command> [args]

			if constexpr ( Usage.empty ( ) ) static_assert ( false, "Usage is empty" );

			if ( auto command = ctre::match< pattern > ( static_cast< std::string_view > ( Usage ) ); command )
				return command.get< 1 > ( ).to_view ( );
			return "";
		}
		return "";
	}
	constexpr std::string_view usage ( ) override
	{
		return Usage;
	}

	// Виртуальная функция для выполнения команды (по умолчанию)
	void execute ( ) override = 0;
};
}
#endif						   // COMMAND_HPP
