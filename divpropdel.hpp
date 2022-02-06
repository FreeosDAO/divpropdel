// This contract is a part of so called '2nd voter solver'. Related variables and functions in the frontend 
// are refeered in the frontend comments as '2nd vote solver'. This contrsct heavily rely on the frontend and should
// be not used without frontend (except testing).
// Important: 'Second voter' is voter who should vote as the second, not the 2nd voter on the list!

#pragma once
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

namespace freedao {
using namespace eosio;
using namespace std;

class[[eosio::contract("divpropdel")]] divpropdel : public contract {

public:

  using contract::contract;

  [[eosio::action]] void version(); 
  [[eosio::action]] void dropmessage(name proposer, name second_voter);
  [[eosio::action]] void div2ndvote( name voter );
  [[eosio::action]] void remove();       


const name div_acct  = "freeosdiv5"_n; // verify and change  // dividenda_account
const uint8_t PROPOSER = 1;
// Note: 'Second voter' is voter who should vote as the second (this may be VOTER_A or VOTER_B), not necessarily the 2nd voter on the list!
const uint8_t VOTER_A  = 2;
const uint8_t VOTER_B  = 3;

  //whitelist_struct is copied to enable remote access to original table in 
  TABLE whitelist_struct {                 //!< whitelist_struct - List of eligible proposer and two voters along with their vote.
      uint64_t idno;                       //!< (1-proposer, 2,3 voters)
      name     user;                       
      uint8_t  vote;                       //!< 0 -n/a  1 - no   2 - yes /     - different than zero if already voted
      uint64_t primary_key() const { return user.value; } 
      uint64_t get_secondary_1() const { return idno; }
  };
  using whitelist_index = eosio::multi_index<"whitelist"_n, whitelist_struct,
    indexed_by<"byidno"_n, const_mem_fun<whitelist_struct, uint64_t, &whitelist_struct::get_secondary_1>>>;


private:

/**
     * auth_vip 
     *
     * @details verify proposer and voters accounts are exists in original dividend contract whitelist table. 
     * 
     * @param[in] user - account name for verification.
     *
     * @param[out] 1 - for proposer, 2 or 3 for voter, 0 - not in the table.
     * 
     * @brief The table 'white_list' is used by the dividend contract to define the context for the proposal creation and voting (Its defines
     *        who is the proposer and the two voters along with storing temporalily vote(s) casted by a given voter 
     *        on the current proposal (used only by the action 'dividenda::proposalvote'). 
     * 
     * @pre Can be called only from the inside of the contract.     Note: This function has no entry from the frontend.
     *
     */
  uint8_t auth_vip( name user ){
  whitelist_index white_list(div_acct, div_acct.value); // looking at original dividenda contract table
  auto v = white_list.find(user.value); // Is the user on the list?
  if (v!=white_list.end()){ // process
    return v->idno;     // user verified 1,2, or 3.
  } 
  return 0;  // user not verified 0.   
  }

  TABLE postbox_struct {                  //   base structure for the trigger table used in this contract.   
      uint64_t key;                       //!< primary key - (always one or no record).
      name eosaccount;                    //!< second voter name received from frontend ('2nd voter solver')
      uint64_t primary_key() const { return 0; } // todo verify
  };
  // typedef eosio::multi_index<"postboxs"_n, postbox_struct> postbox_table;
  using postbox_table = eosio::multi_index<"postboxs"_n, postbox_struct>;

}; // end of contract
} // end of namespace freedao