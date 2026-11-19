# Database Design

## Purpose

The database layer provides persistent storage for the IoT-driven inventory management system.

The database is responsible for maintaining:

- product information,
- current inventory state,
- inventory transaction history,
- and sensor measurements associated with inventory events.

SQLite is used during prototype development on the Raspberry Pi 5.

The database file is:

`inventory.db`

---

## Database Architecture

The current prototype contains three primary tables:

1. `products`
2. `inventory_state`
3. `inventory_transactions`

The relationship is:

```text
products
    │
    ├──────────────→ inventory_state
    │
    └──────────────→ inventory_transactions




## Database Schema V2 – Multi-Item and Multi-Quantity Model

The initial database model associated one RFID UID directly with one product.

This was sufficient for the first prototype but did not support multiple physical units belonging to the same product.

Schema V2 therefore separates:

- product identity,
- physical item identity,
- current aggregate quantity,
- and transaction history.

### Schema V2 Structure

```text
products
    │
    ├──── inventory_items
    │         │
    │         └──── individual RFID-tagged units
    │
    ├──── inventory_state
    │         │
    │         └──── aggregate product quantity
    │
    └──── inventory_transactions
              │
              └──── historical quantity changes

    