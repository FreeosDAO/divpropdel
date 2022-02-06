#include "divpropdel.hpp"
#include <eosio/eosio.hpp>
#include <eosio/system.hpp>

namespace freedao {

using namespace eosio;
using namespace std;

const std::string VERSION = "0.0.1";

// ACTION
void divpropdel::version() {
  std::string version_message = "Version = " + VERSION;

  check(false, version_message);
}


/* ------------------------ drop_message

    Purpose

      Called from frontend - when proposal need to be cancelled but already is once voted. 
      Proposer calls this action to set trigger on and pass to local trigger_table (type postbox_table)
      second_voter name.
    
    Parameters (all verified against original whitelist table)

      name proposer     - requesting proposer 
      name second_voter - request receiver
    
    Operations
    
      - verify parameters against dividenda's whitelist table
      - set second_voter name to local trigger_table  
*/

// ACTION
void divpropdel::dropmessage( 
  name proposer,              // requesting proposer 
  name second_voter )         // request receiver       
{
// remote parameter verification: verify proposer and voter in whitelist table at dividenda contract.
check( (auth_vip(proposer)==PROPOSER), "proposername not authorized by whitelist!" );
check( (auth_vip(second_voter)==VOTER_A)||(auth_vip(second_voter)==VOTER_B), "second_voter not authorized by whitelist!" );
require_auth(proposer);

postbox_table trigger(get_self(), get_self().value);
  auto pro_itr = trigger.begin();
  if( pro_itr == trigger.end() )
  {
     trigger.emplace(get_self(), [&](auto &p) { // This is only first time
        p.key                 = 1;
        p.eosaccount          = second_voter; 
     });
  }
  else
  {
    trigger.modify(pro_itr, get_self(), [&](auto &p) {
        p.key                 = 1;
        p.eosaccount          = second_voter; 
    });
  }     
} 
//
//--- drop_message


/*---------------------- div2ndvote
  
  Purpose

    Called from frontend by second voter to answer proposer message.

  Parameter

    name voter - the caller

  Operations
  
    1. Call remotely voting proposal action on dividenda with pre-set proposal rejection. 
    This is done as second voter.
    2. Reset trigger table.

*/

// ACTION
void divpropdel::div2ndvote( name voter ) {
  check( (auth_vip(voter)==VOTER_A)||(auth_vip(voter)==VOTER_B), "second voter not authorized by whitelist!" );
  require_auth(voter);

  // Call rejection vote in dividenda for second voter
  const uint8_t uservote = 1; // hard pre-set vote reject
  action negativevote = action(
        permission_level{voter, "active"_n},
        name(div_acct),   
        "proposalvote"_n,
        std::make_tuple(get_self(), voter, uservote )
        );
  negativevote.send();
  
  // reset trigger table
  postbox_table trigger(get_self(), get_self().value);
  auto pro_itr = trigger.begin();
  check(pro_itr != trigger.end(), "Trigger Record to remove does not exist.");
    trigger.erase(pro_itr);
}

// ACTION 
void divpropdel::remove()
{
  require_auth( get_self() );
  // reset trigger table
  postbox_table trigger(get_self(), get_self().value);
  auto pro_itr = trigger.begin();
  check(pro_itr != trigger.end(), "Trigger Record to remove does not exist.");
    trigger.erase(pro_itr);
}

/* +-----------------------------------
   +  removewhite -  TODO REWRITE
   +-----------------------------------
             +
             +  Remove completely white_list table.   
             */

// [[eosio::action]]
// void divpropdel::removewhite(){
//  require_auth( _self );
//  whitelist_index white_list(get_self(), get_self().value);
//  // Delete all records in _messages table
//  auto itr = white_list.begin();
//    while (itr != white_list.end()) {
//       itr = white_list.erase(itr);
//    }
//}
//
//---
//==================================================================================
} // end of namespace freedao
