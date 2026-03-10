---
title: "Topic"
---

## Seminar title

Classical Knapsack Cryptosystem and its Variations

## One-sentence focus

This project studies the classical Merkle-Hellman knapsack cryptosystem, its known weaknesses, and selected improved variants.

## Goals

1. Explain the classical scheme (key generation, encryption, decryption).
2. Analyze cryptanalytic weaknesses.
3. Compare selected variations by security and implementation complexity.

## Annotation

The work starts from the original Merkle-Hellman construction based on subset-sum hardness and documents how the trapdoor is built from superincreasing sequences and modular transformation. It then focuses on why the classical variant is insecure in practice, with emphasis on structural weaknesses used by known attacks. Finally, it evaluates selected modifications and variants, comparing their security claims, practical implementability, and educational value for understanding public-key design trade-offs.

Next steps concentrate on showing, directly on parameters and in code, how the classical scheme can be broken, mapping out the main variants together with their known attacks/weaknesses, and calling out the computational complexity of every method (NP-hard general case versus polynomial-time superincreasing case, plus time/space costs of attacks).
