# bt bondstate Machine

蓝牙连接状态机状态切换

# 参考文档

* [蓝牙状态机（StateMachine）](https://blog.csdn.net/qq_43824618/article/details/113655333)


# 状态切换

BondStateMachine中状态只有两种:StableState和PendingCommandState两种状态，初始状态是StatbleState。


# 状态切换日志

```
07-04 17:54:04.656 10132 10178 D BluetoothBondStateMachine: StableState msg.what=1
07-04 17:54:04.658 10132 10178 I BluetoothBondStateMachine: Bond address is:DC:2C:26:EA:AF:78
07-04 17:54:04.667 10132 10178 I BluetoothBondStateMachine: Entering PendingCommandState State
07-04 17:54:04.671 10132 10167 I BluetoothBondStateMachine: bondStateChangeCallback: Status: 0 Address: DC:2C:26:EA:AF:78 newState: 1 hciReason: 0
07-04 17:54:04.672 10132 10178 D BluetoothBondStateMachine: PendingCommandState msg.what=4
07-04 17:54:04.678 10132 10178 I BluetoothBondStateMachine: Bond State Change Intent:DC:2C:26:EA:AF:78 BOND_NONE => BOND_BONDING
07-04 17:54:07.872 10132 10167 I BluetoothBondStateMachine: sspRequestCallback: [B@b5afdd4 name: [B@50cdb7d cod: 9536 pairingVariant 2 passkey: 542219,isAutoConfirm false
07-04 17:54:07.877 10132 10178 D BluetoothBondStateMachine: PendingCommandState msg.what=5



07-04 17:54:20.973 10132 10167 I BluetoothBondStateMachine: bondStateChangeCallback: Status: 0 Address: DC:2C:26:EA:AF:78 newState: 2 hciReason: 0
07-04 17:54:20.973 10132 10178 D BluetoothBondStateMachine: PendingCommandState msg.what=4
07-04 17:54:20.984 10132 10178 I BluetoothBondStateMachine: DC:2C:26:EA:AF:78 is bonded, wait for SDP complete to broadcast bonded intent
07-04 17:54:20.984 10132 10178 I BluetoothBondStateMachine: StableState(): Entering Off State
07-04 17:54:21.263 10132 10178 D BluetoothBondStateMachine: StableState msg.what=10
07-04 17:54:21.272 10132 10178 I BluetoothBondStateMachine: Bond State Change Intent:DC:2C:26:EA:AF:78 BOND_BONDING => BOND_BONDED
07-04 17:54:23.988 10132 10178 D BluetoothBondStateMachine: StableState msg.what=11
```