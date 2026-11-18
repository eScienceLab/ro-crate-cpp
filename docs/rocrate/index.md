---
generator: doxide
---


# rocrate



## Types

| Name | Description |
| ---- | ----------- |
| [Entity](Entity.md) | Represents an entity in the RO-Crate. |
| [ROCrate](ROCrate.md) | Represents an RO-Crate, a structured collection of entities and metadata. |

## Functions

| Name | Description |
| ---- | ----------- |
| [error_message](#error_message) | Returns the message template for the given error code. |

## Function Details

### error_message<a name="error_message"></a>
!!! function "constexpr std::string_view error_message(ErrorCode code)"

    Returns the message template for the given error code.
    Templates with placeholders are substituted via std::format at the throw site.
    

