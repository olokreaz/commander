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
			constexpr auto pattern = ctll::fixed_string { R"(^(\w+).*)" };

			if ( fmt.empty ( ) ) return "";

			if ( auto command = ctre::match< pattern > ( static_cast< std::string_view > ( fmt ) ); command )
				return command.get< 1 > ( ).to_view ( );
			return "";
		}
	} //                                                                      namespace detail

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
		using args_t = std::tuple< Args... >;
		using ctx_t  = typename Interface::context_t;

		ctx_t  m_context;
		args_t m_args;

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
			return detail::extract_name ( FMT );
		}
		constexpr virtual inline std::string_view usage ( ) override
		{
			return FMT;
		}
		virtual inline int execute ( ) override = 0;
	};

	template< class... Context > class ExecuterCommands
	{
	public:
		using ICommand							   = InterfaceCommand< Context... >;
		using PtrCommand						   = ICommand*;
		template< fixstr::fixed_string FMT, class... Args > using CCommand = ImplCommand< ICommand, FMT, Args... >;

		using CommandMap = std::unordered_map< std::string_view, PtrCommand >;

		using iterator	     = typename CommandMap::iterator;
		using const_iterator = typename CommandMap::const_iterator;
		using pointer	     = typename CommandMap::pointer;
		using const_pointer  = typename CommandMap::const_pointer;

	private:
		CommandMap m_registered_commands;

	public:
		ExecuterCommands ( ) = default;

		template< class Type > inline void create_command ( )
		{
			PtrCommand pcom				      = new Type;
			this->m_registered_commands[ pcom->name ( ) ] = pcom;
		}
		template< class... Types > inline void create_commands ( )
		{
			( create_command< Types > ( ), ... );
		}

		PtrCommand operator[] ( const std::string_view v )
		{
			if ( auto it = m_registered_commands.find ( v ); it != m_registered_commands.end ( ) ) return it->second;
			return nullptr;
		}

		~ExecuterCommands ( )
		{
			for ( const auto& [ name, pCommand ] : m_registered_commands ) delete pCommand;
		}
	};

} //	namespace ox

#endif // COMMAND_HPP
