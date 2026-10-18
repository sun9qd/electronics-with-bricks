# Electronics With Bricks: System Principle

The Electronics-With-Bricks construction kit is based on a clearly defined core concept. The underlying terms are explained below:

## Studs

The construction kit allows for the assembly of electronic circuits based on an 8 mm grid. This corresponds to the stud spacing found on standard toy building plates available in retail stores. The kit includes components—referred to as "bricks"—each of which covers a rectangular area of ​​studs. Consequently, these bricks can be categorized based on their dimensions (length and width) in terms of the number of studs they cover. Available sizes include 1x1, 2x1, 2x2, 3x1, and so on. The longer dimension is always specified first (e.g., 3x2 rather than 2x3).

## Pads

Each building block features external contacts that allow for an electrical connection to be established with neighboring blocks. These contacts each cover a single stud; the electrical equivalent is a copper surface located within the block, positioned over a stud. This internal copper surface is referred to as a "pad." This determines the maximum number of distinct external contacts a block can possess. For example, a 3x3 block has 8 pads along its outer perimeter, meaning it can have a maximum of 8 separate electrical connections to other blocks. The central stud—out of the 3x3 (9) total studs covered—is an internal stud and does not function as a pad in the context of this system.

The blocks can house any type of internal electronic circuitry and wiring, entirely independent of the pads, which serve only to define the block's external interface. Consequently, a 2x1 brick—which provides only two pads for external connectivity—could contain highly complex internal electronics linking those two pads.

A single pad can connect to multiple neighboring blocks via various connectors; however, in such cases, these all constitute the same electrical connection. In other words, the connectors associated with a given pad are electrically linked to one another.

## Connectors

The connectors are individual pins soldered onto pads, enabling contact with adjacent modules. They are positioned at a specific distance from the edge of the module, calculated so that the gap between a connector and a corresponding connector on an adjacent module is exactly 1/10 of an inch (2.54 mm). Consequently, standard jumper plugs can be used to establish electrical connections between the modules.

Each connector facilitates a connection in a specific direction: left, right, up, or down. The number of connectors available on a given pad depends on how many outer edges that pad possesses. For instance, a 1x1 module has four outer edges; therefore, its single pad can support connections in all four directions—meaning this pad is equipped with four connectors. A 3x1 module differs: the pads at either end have three potential neighbors and thus feature three connectors, while the pad in the center has two connectors.

The numbers of pads and connectors described here always represent maximum values. In other words, while a module *can* implement these pads or connectors, it is not required to do so. For example, a 3x3 module might implement only 6 of its 8 potential pads, and not every possible connector position may be populated.

---

Copyright (c) 2024-2026 sun9qd


