# Bluetooth CID Allocate

BlueDroid CID动态分配原理

# 参考文档

# CID动态分配

  * system/bt/stack/include/l2cdefs.h
    ```
    /* L2CAP Predefined CIDs
    */
    enum : uint16_t {
      L2CAP_SIGNALLING_CID = 1,
      L2CAP_CONNECTIONLESS_CID = 2,
      L2CAP_AMP_CID = 3,
      L2CAP_ATT_CID = 4,
      L2CAP_BLE_SIGNALLING_CID = 5,
      L2CAP_SMP_CID = 6,
      L2CAP_SMP_BR_CID = 7,
      L2CAP_BASE_APPL_CID = 0x0040,
    };
    ```
  * system/bt/stack/l2cap/l2c_main.cc
    ```
    /*******************************************************************************
     *
     * Function         l2c_rcv_acl_data
     *
     * Description      This function is called from the HCI Interface when an ACL
     *                  data packet is received.
     *
     * Returns          void
     *
     ******************************************************************************/
    void l2c_rcv_acl_data(BT_HDR* p_msg) {
      // ...省略
    
      /* Send the data through the channel state machine */
      if (rcv_cid == L2CAP_SIGNALLING_CID) {
        process_l2cap_cmd(p_lcb, p, l2cap_len);
        osi_free(p_msg);
        return;
      }
    
      // ...省略
    }
    ```
  * system/bt/stack/l2cap/l2c_main.cc
    ```
    /*******************************************************************************
     *
     * Function         process_l2cap_cmd
     *
     * Description      This function is called when a packet is received on the
     *                  L2CAP signalling CID
     *
     * Returns          void
     *
     ******************************************************************************/
    static void process_l2cap_cmd(tL2C_LCB* p_lcb, uint8_t* p, uint16_t pkt_len) {
      // ...省略
    
      switch (cmd_code) {
        // ...省略
    
        case L2CAP_CMD_CONN_REQ: {
          // ...省略
    
          tL2C_CCB* p_ccb = l2cu_allocate_ccb(p_lcb, 0);
          if (p_ccb == nullptr) {
            LOG_ERROR("Unable to allocate CCB");
            l2cu_reject_connection(p_lcb, rcid, id, L2CAP_CONN_NO_RESOURCES);
            break;
          }
          p_ccb->remote_id = id;
          p_ccb->p_rcb = p_rcb;
          p_ccb->remote_cid = rcid;
          p_ccb->connection_initiator = L2CAP_INITIATOR_REMOTE;
    
          // ...省略
    
          l2c_csm_execute(p_ccb, L2CEVT_L2CAP_CONNECT_REQ, &con_info);
          break;
        }
    
        // ...省略
      }
    
      // ...省略
    }
    ```
  * system/bt/stack/l2cap/l2c_utils.cc
    ```
    /*******************************************************************************
     *
     * Function         l2cu_allocate_ccb
     *
     * Description      This function allocates a Channel Control Block and
     *                  attaches it to a link control block. The local CID
     *                  is also assigned.
     *
     * Returns          pointer to CCB, or NULL if none
     *
     ******************************************************************************/
    tL2C_CCB* l2cu_allocate_ccb(tL2C_LCB* p_lcb, uint16_t cid) {
      // ...省略
    
      /* Get a CID for the connection */
      p_ccb->local_cid = L2CAP_BASE_APPL_CID + (uint16_t)(p_ccb - l2cb.ccb_pool);
    
      // ...省略
    }
    
    ``` 
  * 如上可知，除了固定的CID，应用的CID是设备连接功能的时候进行动态创建的，大于等于L2CAP_BASE_APPL_CID；

