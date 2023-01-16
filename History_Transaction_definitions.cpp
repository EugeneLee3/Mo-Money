#include <cassert>
#include <iomanip>
#include <iostream>

#include "project4.hpp"
#include "History.hpp"
#include "Transaction.hpp"

////////////////////////////////////////////////////////////////////////////////
// Definitions for Transaction class
////////////////////////////////////////////////////////////////////////////////
//
//

// Constructor
// TASK 1
//
Transaction::Transaction( std::string ticker_symbol, unsigned int day_date, unsigned int month_date, unsigned year_date,
bool buy_sell_trans,  unsigned int number_shares, double trans_amount):
  symbol{ticker_symbol},
  day{day_date},
  month{month_date},
  year{year_date},
  trans_type{(buy_sell_trans) ? "Buy" : "Sell"},
  shares{number_shares},
  amount{trans_amount},
  p_next{nullptr} {
    trans_id = assigned_trans_id++;
    acb = 0;
    acb_per_share = 0;
    share_balance = 0;
    cgl = 0;
  }


// Destructor
// TASK 1
//
Transaction::~Transaction() {
  delete p_next;
}


// Overloaded < operator.
// TASK 2
//
bool Transaction::operator<( Transaction const &other ) {
  if (year < other.get_year()) {
    return true;
  } else {
    if (month < other.get_month() && year == other.year) {
      return true;
    } else {
      if (day < other.get_day() && month == other.month && year == other.year) {
        return true;
      }
      else if (year == other.get_year() && month == other.get_month() && day == other.get_day()) {
        if (trans_id > other.get_trans_id()) {
          return true;
        }
      } 
    }
  }
  return false;
}


// GIVEN
// Member functions to get values.
//
std::string Transaction::get_symbol() const { return symbol; }
unsigned int Transaction::get_day() const { return day; }
unsigned int Transaction::get_month() const { return month; }
unsigned int Transaction::get_year() const { return year; }
unsigned int Transaction::get_shares() const { return shares; }
double Transaction::get_amount() const { return amount; }
double Transaction::get_acb() const { return acb; }
double Transaction::get_acb_per_share() const { return acb_per_share; }
unsigned int Transaction::get_share_balance() const { return share_balance; }
double Transaction::get_cgl() const { return cgl; }
bool Transaction::get_trans_type() const { return (trans_type == "Buy") ? true: false ; }
unsigned int Transaction::get_trans_id() const { return trans_id; }
Transaction *Transaction::get_next() { return p_next; }

// GIVEN
// Member functions to set values.
//
void Transaction::set_acb( double acb_value ) { acb = acb_value; }
void Transaction::set_acb_per_share( double acb_share_value ) { acb_per_share = acb_share_value; }
void Transaction::set_share_balance( unsigned int bal ) { share_balance = bal ; }
void Transaction::set_cgl( double value ) { cgl = value; }
void Transaction::set_next( Transaction *p_new_next ) { p_next = p_new_next; }

// GIVEN
// Print the transaction.
//
void Transaction::print() {
  std::cout << std::fixed << std::setprecision(2);
  std::cout << std::setw(4) << get_trans_id() << " "
    << std::setw(4) << get_symbol() << " "
    << std::setw(4) << get_day() << " "
    << std::setw(4) << get_month() << " "
    << std::setw(4) << get_year() << " ";


  if ( get_trans_type() ) {
    std::cout << "  Buy  ";
  } else { std::cout << "  Sell "; }

  std::cout << std::setw(4) << get_shares() << " "
    << std::setw(10) << get_amount() << " "
    << std::setw(10) << get_acb() << " " << std::setw(4) << get_share_balance() << " "
    << std::setw(10) << std::setprecision(3) << get_acb_per_share() << " "
    << std::setw(10) << std::setprecision(3) << get_cgl()
    << std::endl;
}


////////////////////////////////////////////////////////////////////////////////
// Definitions for the History class
////////////////////////////////////////////////////////////////////////////////
//
//


// Constructor
// TASK 3 - Week 2
//
History::History(): 
  p_head{nullptr} {
}


// Destructor
// TASK 3 - Week 2
//
History::~History() {
  delete p_head;
}


// read_history(...): Read the transaction history from file.
// TASK 4 - Week 2
//
void History::read_history() {
  ece150::open_file();
  while (ece150::next_trans_entry()) {
    Transaction * trans{p_head};
    insert(new Transaction(ece150::get_trans_symbol(), ece150::get_trans_day(), ece150::get_trans_month(),
    ece150::get_trans_year(), ece150::get_trans_type(), ece150::get_trans_shares(), ece150::get_trans_amount()));
  }
  ece150::close_file();
}


// insert(...): Insert transaction into linked list.
// TASK 5 - Week 2
//
void History::insert( Transaction *p_new_trans ) {
  Transaction* temp_trans{p_head};
  if (p_head == nullptr) {
    p_head = p_new_trans;
  }
  else {
    while(temp_trans->get_next() != nullptr) {
      temp_trans = temp_trans->get_next();
    }
    temp_trans->set_next(p_new_trans);
  }
}


// sort_by_date(): Sort the linked list by trade date.
// TASK 6
//
void History::sort_by_date() {
  Transaction *sorted{nullptr};
  Transaction *trans{p_head};

  while (trans != nullptr) {
    Transaction *next_trans{trans->get_next()};

    if (sorted == nullptr || *trans < *sorted) {
      trans->set_next(sorted);
      sorted = trans;
    }
    else {
      Transaction *current{sorted};
      while (current->get_next() != nullptr && *current->get_next() < *trans) {
        current = current->get_next();
      }
      trans->set_next(current->get_next());
      current->set_next(trans);
    }
    trans = next_trans;
  }
  p_head = sorted;
}



// update_acb_cgl(): Updates the ACB and CGL values.
// TASK 7
//
void History::update_acb_cgl() {
  Transaction* trans{p_head->get_next()};
  Transaction* prev{p_head};
  
  prev->set_acb(prev->get_amount());
  prev->set_share_balance(prev->get_shares());
  prev->set_acb_per_share(prev->get_acb() / prev->get_share_balance());

  while (trans != nullptr) {
    if (trans->get_trans_type()) {
      trans->set_acb(prev->get_acb() + trans->get_amount());
      trans->set_share_balance(prev->get_share_balance() + trans->get_shares());
      trans->set_acb_per_share(trans->get_acb() / trans->get_share_balance());
    }
    else {
      trans->set_acb(prev->get_acb() - trans->get_shares() * prev->get_acb_per_share());
      trans->set_share_balance(prev->get_share_balance() - trans->get_shares());
      trans->set_acb_per_share(trans->get_acb() / trans->get_share_balance());
      trans->set_cgl(trans->get_amount() - trans->get_shares()*prev->get_acb_per_share());
    }
    trans = trans->get_next();
    prev = prev->get_next();
  }

}


// compute_cgl(): )Compute CGL.
// TASK 8
double History::compute_cgl( unsigned int year ) {
  Transaction* trans{p_head};
  double year_cgl{};

  while (trans != nullptr) {
    if (trans->get_year() == year) {
      year_cgl += trans->get_cgl();
    }
    trans = trans->get_next();
  }

  return year_cgl;
}


// print() Print the transaction history.
//TASK 9 - Week 2
//  
void History::print() {
  Transaction* temp_trans{p_head};  
  int counter{};
  std::cout << "========== BEGIN TRANSACTION HISTORY ============" << std::endl;
  while (temp_trans != nullptr) {
    std::cout << counter << '\t' << 
      temp_trans->get_symbol() << '\t' <<
      temp_trans->get_day() << '\t' <<
      temp_trans->get_month() << '\t' <<
      temp_trans->get_year() << '\t' <<
      ((temp_trans->get_trans_type()) ? "Buy":"Sell") << '\t' <<
      temp_trans->get_shares() << '\t' <<
      temp_trans->get_amount() << '\t' <<

      temp_trans->get_acb() << '\t' <<
      temp_trans->get_share_balance() << '\t' <<
      temp_trans->get_acb_per_share() << '\t' <<
      temp_trans->get_cgl() << std::endl;
    temp_trans = temp_trans->get_next();
    ++counter;
  }
  std::cout << "========== END TRANSACTION HISTORY ============" << std::endl;
}


// GIVEN
// get_p_head(): Full access to the linked list.
//
Transaction *History::get_p_head() { return p_head; }
