#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <scn/scan.h>
#include <ctre.hpp>
#include <fixed_string.hpp>
#include <string>
#include <tuple>
#include <utility>

namespace ox {
	namespace detail {
		constexpr std::string_view extract_name ( const std::string_view fmt )
		{
			constexpr auto pattern = ctll::fixed_string { R"(^(\w+).*)" }; // example: <prefix><command> [args]

			if ( fmt.empty ( ) ) return "";

			if ( auto command = ctre::match< pattern > ( static_cast< std::string_view > ( fmt ) ); command )
				return command.get< 1 > ( ).to_view ( );
			return "";
		}

		template< class... Args > class InterfaceCommand
		{
		public:
			using context_t = std::tuple< Args... >;

		protected:
			InterfaceCommand ( ) = default;

		public:
			virtual ~			   InterfaceCommand ( )			 = default;
			virtual bool			   parser ( const std::string_view ctx ) = 0;
			virtual void			   set_context ( context_t&& ctx = { } ) = 0;
			virtual int			   execute ( )				 = 0;
			virtual constexpr std::string_view name ( )				 = 0;
			virtual constexpr std::string_view usage ( )				 = 0;
		};

		template< class Interface, fixstr::fixed_string FMT, class... Args > class ImplCommand : public Interface
		{
		public:
			using args_t	  = std::tuple< Args... >;
			using ctx_t	  = typename Interface::context_t;
			using interface_t = Interface;

			ctx_t  m_context;
			args_t m_args;

			constexpr std::string_view m_name = detail::extract_name ( FMT );

		protected:
			ImplCommand ( ) = default;

			const ctx_t& context ( )
			{
				return m_context;
			}
			const args_t& args ( )
			{
				return m_args;
			}

		public:
			constexpr inline bool parser ( const std::string_view ctx ) override
			{
				if constexpr ( FMT.size ( ) != 0 && sizeof...( Args ) != 0 ) {
					if ( auto result = scn::scan< Args... > ( ctx, FMT ) ) {
						m_args = std::move ( result->value ( ) );
						return true;
					}
					return false;
				} else {
					return true;
				}
			}

			inline void set_context ( ctx_t&& ctx ) override
			{
				if constexpr ( std::tuple_size_v< decltype ( m_context ) > ) m_context = std::move ( ctx );
			}
			constexpr virtual inline std::string_view name ( ) override
			{
				return m_name;
			}
			constexpr virtual inline std::string_view usage ( ) override
			{
				return FMT;
			}
			virtual inline int execute ( ) override = 0;
		};

	} //                                                                      namespace detail

	// template< fixstr::fixed_string FMT, class... Args > class ImplCommand< InterfaceCommand<>, FMT, Args... >
	// { };
	template< class... Context > class ExecuterCommands
	{
	public:
		using ICommand							   = detail::InterfaceCommand< Context... >;
		using RawPtrCommand						   = ICommand*;
		using PtrCommand						   = std::unique_ptr< ICommand >;
		template< fixstr::fixed_string FMT, class... Args > using CCommand = detail::ImplCommand< ICommand, FMT, Args... >;

		using CommandMap = std::unordered_map< std::string_view, PtrCommand >;

		using iterator	     = typename CommandMap::iterator;
		using const_iterator = typename CommandMap::const_iterator;
		using pointer	     = typename CommandMap::pointer;
		using const_pointer  = typename CommandMap::const_pointer;

	private:
		CommandMap m_registered_commands;

	public:
		ExecuterCommands ( ) = default;

		inline void create_command ( PtrCommand pcom )
		{
			assert ( pcom );
			this->m_registered_commands[ pcom->name ( ) ] = std::move ( pcom );
		}

		template< class Type, class... Args > inline void create_command ( const Args& args )
		{
			this->create_command ( std::make_unique< Type > ( args... ) );
		}

		template< class... Types > inline void create_commands ( )
		{
			( create_command< Types > ( ), ... );
		}

		inline RawPtrCommand operator[] ( const std::string_view v )
		{
			if ( const auto it = m_registered_commands.find ( v ); it != m_registered_commands.end ( ) ) return it->second.get ( );
			return nullptr;
		}
	};

} //                                                                      namespace ox
// // Базовый интерфейс команды
// class ICommand
// {
// 	static inline std::string m_prefix = "";
//
// protected:
// 	ICommand ( ) = default;
//
// public:
// 	static auto prefix ( ) -> std::string_view
// 	{
// 		return m_prefix;
// 	}
//
// 	static void prefix ( const std::string& prefix )
// 	{
// 		m_prefix = prefix;
// 	}
//
// 	virtual ~ICommand ( ) = default;
//
// 	// Виртуальная функция для выполнения команды
// 	virtual void execute ( ) = 0;
//
// 	// Виртуальная функция для парсинга
// 	virtual bool parse ( std::string&& cmd ) = 0;
//
// 	virtual constexpr std::string_view name ( )  = 0;
// 	virtual constexpr std::string_view usage ( ) = 0;
// };
//
// // Шаблонный класс команды
// template< fixstr::fixed_string Usage, class... Args > class CCommand : public ICommand
// {
// public:
// 	using args_t = std::tuple< Args... >;
//
// private:
// 	thread_local inline static args_t m_args;
//
// protected:
// 	args_t& args ( )
// 	{
// 		return m_args;
// 	}
//
// public:
// 	~CCommand ( ) override = default;
//
// 	// Шаблонный метод для парсинга, если Usage не пуст
// 	bool parse ( std::string&& cmd ) override
// 	{
// 		if constexpr ( Usage.size ( ) != 0 && sizeof...( Args ) != 0 ) {
// 			if ( auto result = scn::scan< Args... > ( cmd, Usage ) ) {
// 				m_args = std::move ( result->value ( ) );
// 				LOGD << "Parsed: " << fmt::format ( "{}", m_args );
// 				return true;
// 			}
// 			LOGE << "Failed to parse command: " << cmd;
// 			return false;
// 		} else {
// 			return true;
// 		}
// 	}
//
// 	constexpr std::string_view name ( ) override
// 	{
// 		{
// 			constexpr auto pattern = ctll::fixed_string { R"(^(\w+).*)" }; // example: <prefix><command> [args]
//
// 			if constexpr ( Usage.empty ( ) ) static_assert ( false, "Usage is empty" );
//
// 			if ( auto command = ctre::match< pattern > ( static_cast< std::string_view > ( Usage ) ); command )
// 				return command.get< 1 > ( ).to_view ( );
// 			return "";
// 		}
// 		return "";
// 	}
// 	constexpr std::string_view usage ( ) override
// 	{
// 		return Usage;
// 	}
//
// 	// Виртуальная функция для выполнения команды (по умолчанию)
// 	void execute ( ) override = 0;
// };
//
// class CJoke final : public CCommand< "joke" >
// {
// public:
// 	     CJoke ( ) = default;
// 	void execute ( ) override
// 	{
// 		LOGI << "Joke command executed";
// 	}
//
// 	~CJoke ( ) override = default;
// };
//
// class CPlayerPing final : public CCommand< "ping {}", std::string >
// {
// public:
// 	// Переопределённый метод execute для команды Ping
// 	void execute ( ) override
// 	{
// 		const auto& [ player ] = args ( ); // Получаем значение из кортежа
// 		LOGI << "Pinging player: @" << player;
// 	}
//
// 	~CPlayerPing ( ) override = default;
// };

#endif // COMMAND_HPP
