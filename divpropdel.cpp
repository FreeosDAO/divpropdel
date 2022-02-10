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

      Called from frontend - when proposal need to be cancelled but already was once voted. 
      Proposer calls this action to set trigger on and pass to local trigger_table (type postbox_table)
      second_voter name.
    
    Parameters (all verified against original whitelist table)

      name proposer     - requesting proposer 
      name second_voter - request receiver
    
    Operations
    
      - verify parameters against dividenda's 'whitelist' table
      - set second_voter name to local 'trigger_table' (postboxs) 
*/

// ACTION
void divpropdel::dropmessage( 
  name proposer,              // requesting proposer 
  name second_voter )         // request receiver       
{
// both parameters verification: verify proposer and voter in whitelist table at dividenda contract.
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
void divpropdel::div2ndvote( name votername ) {
  check( (auth_vip(votername)==VOTER_A)||(auth_vip(votername)==VOTER_B), "second voter not authorized by whitelist!" );
  require_auth(votername);

  // Call rejection vote in dividenda for second voter
  const uint8_t vote = 1; // hard pre-set vote reject
  
  action negativevote = action(
        permission_level{"votername"_n, "active"_n},
        name(div_acct),   
        "proposalvote"_n,
        std::make_tuple(votername, vote) // note: it must be used the same names like names of target parameters (??)
        );
  negativevote.send();
  
  // reset trigger table
  postbox_table trigger(get_self(), get_self().value);
  auto pro_itr = trigger.begin();
  check(pro_itr != trigger.end(), "Trigger Record to remove does not exist.");
    trigger.erase(pro_itr);
}
// ===

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

// Querying account type (0-3) for the frontend.
// Answer is send through notify_front().   - PROVIDED IN THE MEANS OF TEST
//
// ACTION
void divpropdel::query( name eosaccount )
{
  require_auth( eosaccount );
  // Clean up notify_front
  clearfront();                                                   
  // Identify account on whitelist (1,2, or 3) else (0)
  uint8_t answer;
  answer = auth_vip(eosaccount);
  notify_front(answer);
}  
//
//---

  /*
   +-----------------------------------
   +  notify_front -  
   +-----------------------------------
             +
             +  Build up a queue of warning messages for frontend.    
             */

void divpropdel::notify_front( uint8_t number ) 
{
  messages_table errortable( get_self(), get_self().value );                                  
  auto ee = errortable.emplace( get_self(), [&](auto &e) {    
    e.key = errortable.available_primary_key(); 
    e.errorno = number;
  } );                                                                                      
} 
//---


// Clear frontend notification buffer created previously by notify_front    
// ACTION
void divpropdel::clearfront() {
    messages_table    errortable(get_self(), get_self().value);
    auto   rec_itr  = errortable.begin();
    while (rec_itr != errortable.end()) {
           rec_itr  = errortable.erase(rec_itr);
    }
} 
//
//---

//==================================================================================
} // end of namespace freedao


