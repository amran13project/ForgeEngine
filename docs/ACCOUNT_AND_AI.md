# Forge Account + AI

## Account

The native editor starts at Login when a local account exists, otherwise Signup. Sign-up collects display name, email, password, date of birth and country/region. The local provider stores only a password fingerprint, never the clear-text password.

For production public services, replace the local provider with a hosted authentication implementation behind the account boundary.

## Birthday

The account service checks the machine's local date. When the day/month matches the stored date of birth, Forge displays a birthday greeting. Date of birth is account data and is not sent to the AI layer.

## AI

ForgeAI exposes stable execution modes and a change-plan boundary. The engine can later connect local, cloud or enterprise models without rewriting editor workflows. Destructive or broad actions can require confirmation.
