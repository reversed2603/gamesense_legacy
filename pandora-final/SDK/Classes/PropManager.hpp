#pragma once

#include "../sdk.hpp"

namespace Engine
{

  class PropManager
  {
  public:
	 ~PropManager( );
     struct netvar_table {
        std::string name;
        RecvProp* prop;
        uint32_t offset;
        std::vector< RecvProp* > child_props;
        std::vector< netvar_table > child_tables;

        bool operator==( const char* str ) const {
           return name.compare( str ) == 0;
        }
     };
  public:
	 bool Create( IBaseClientDLL* client );
     
	 void Destroy( );

	 int GetOffset( const std::string& table_name, const std::string& prop_name );
	 RecvVarProxyFn Hook( const RecvVarProxyFn hooked, const std::string& table_name, const std::string& prop_name );

	 int GetProp( const std::string& table_name, const std::string& prop_name, RecvProp** out_prop = nullptr );
	 int GetProp( RecvTable* table, const std::string& prop_name, RecvProp** out_prop = nullptr );

	 RecvTable* GetTable( const std::string& name );
     netvar_table LoadTable( RecvTable* recvTable );

     int GetClientID( std::string networkname );

     std::vector< netvar_table > database;
     std::unordered_map< std::string, int > m_client_ids;
  private:
	 std::vector<RecvTable*> m_pTables;

  };

  extern PropManager g_PropManager;

}
