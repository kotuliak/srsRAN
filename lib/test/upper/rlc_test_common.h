/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSLTE_RLC_TEST_COMMON_H
#define SRSLTE_RLC_TEST_COMMON_H

#include "srslte/common/byte_buffer.h"
#include "srslte/interfaces/ue_pdcp_interfaces.h"
#include "srslte/interfaces/ue_rrc_interfaces.h"
#include <vector>

namespace srslte {

class rlc_um_tester : public srsue::pdcp_interface_rlc, public srsue::rrc_interface_rlc
{
public:
  rlc_um_tester() {}

  // PDCP interface
  void write_pdu(uint32_t lcid, unique_byte_buffer_t sdu)
  {
    // check length
    if (lcid != 3 && sdu->N_bytes != expected_sdu_len) {
      printf("Received PDU with size %d, expected %d. Exiting.\n", sdu->N_bytes, expected_sdu_len);
      exit(-1);
    }

    // check content
    uint8_t first_byte = *sdu->msg;
    for (uint32_t i = 0; i < sdu->N_bytes; i++) {
      if (sdu->msg[i] != first_byte) {
        printf("Received corrupted SDU with size %d. Exiting.\n", sdu->N_bytes);
        srslte_vec_fprint_byte(stdout, sdu->msg, sdu->N_bytes);
        exit(-1);
      }
    }

    // srslte_vec_fprint_byte(stdout, sdu->msg, sdu->N_bytes);
    sdus.push_back(std::move(sdu));
  }
  void write_pdu_bcch_bch(unique_byte_buffer_t sdu) {}
  void write_pdu_bcch_dlsch(unique_byte_buffer_t sdu) {}
  void write_pdu_pcch(unique_byte_buffer_t sdu) {}
  void write_pdu_mch(uint32_t lcid, srslte::unique_byte_buffer_t sdu) { sdus.push_back(std::move(sdu)); }
  void notify_delivery(uint32_t lcid, const std::vector<uint32_t>& pdcp_sns) {}
  void notify_failure(uint32_t lcid, const std::vector<uint32_t>& pdcp_sns) {}

  // RRC interface
  void        max_retx_attempted() {}
  std::string get_rb_name(uint32_t lcid) { return std::string(""); }
  void        set_expected_sdu_len(uint32_t len) { expected_sdu_len = len; }

  uint32_t get_num_sdus() { return sdus.size(); }

  // TODO: this should be private
  std::vector<unique_byte_buffer_t> sdus;
  uint32_t                          expected_sdu_len = 0;
};

} // namespace srslte

#endif // SRSLTE_RLC_TEST_COMMON_H
