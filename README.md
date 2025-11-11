# BAKALARKA

## TODO

- superincreasing knapasack porovnanie z normalny preco np jeden a druhy nie
- mnozina vah, -> jedno / viac rieseni 
- priklad -> ilustracny priklad
- animacie mozno
- atacky, fixy
- 12.november (11:30) vynimocne

## THESIS

### OUTLINE
Abstract (SK/EN)
Introduction
1. Theoretical Background
   1. Basics of Cryptography
   2. Knapsack Problem
   3. Knapsack Cryptosystem
2. Implementation and Methodology
   1. Choice of Language
   2. Algorithm Design
   3. Testing
3. Variations and Improvements
   1. Weaknesses
   2. Enhanced Versions
   3. Comparison
4. Results and Discussion
Conclusion
Bibliography
Appendices

### INTRODUCTION (≈ 1–2 pages, unnumbered chapter)

Introduce the reader to the problem and motivation:
- Importance of cryptography and public-key systems
- Why the Knapsack cryptosystem is interesting (historical and educational value)
- Describe the aim and goals of your thesis
- Outline of chapters (“The thesis is structured as follows…”)

### THEORY 

1. Theoretical Background
    1. Basics of Cryptography
        -	Symmetric vs. asymmetric encryption
        -	Mathematical foundations (modular arithmetic, one-way functions, NP problems)

    2. The Knapsack Problem
        -	Definition and mathematical formulation (subset-sum problem)
        -	Complexity and NP-completeness
        -	Example with small numbers

    3. Public Key Cryptosystems and the Merkle–Hellman Knapsack
        -	Idea of public-key encryption
        -	Description of the classical Merkle–Hellman cryptosystem
        -	Key generation, encryption, decryption
        -	Example walk-through

- (You can take content from Alexander Stanoyevitch, Chapter 4.5)

2. Implementation and Methodology
    1. Choice of Programming Language
        -	Which language (Python, C++, etc.)
        -	Why you chose it (libraries, performance, readability)

    2. Design of Implementation
        -	Data structures and algorithm design
        -	Pseudocode of key steps (key generation, encryption, decryption)
        -	Code snippets in listings

    3. Testing and Verification
        -	Test cases for correctness
        -	Example encrypt/decrypt sequences
        -	Timing or performance comparison

3. Variations and Improvements
    1. Weaknesses of the Classical Knapsack Cryptosystem
        -	Shamir’s attack
        -	Density of knapsack and its effect on security

    2. Improved Knapsack Schemes
        -	Multiple superincreasing sequences
        -	Modular transformations
        -	Randomized permutations
        -	Other approaches (Lattice-based variants, if you want to touch modern crypto)

    3. Comparison
        -	Security strength and weaknesses
        -	Efficiency and implementation complexity
        -	Table summarizing differences

4. Results and Discussion
	-	Present what you achieved:
	-	Working implementation
	-	Experimental results
	-	Comparison of variants
	-	Discuss limitations and what could be improved
	-	Relate your results to existing literature

### CONCLUSION

- Summarize what you learned and achieved
- Evaluate if the aims were fulfilled
- Mention possible directions for future work
- (e.g., post-quantum cryptography, hybrid systems)

### APPENDICES

- Appendix A: Source code
- Appendix B: User guide / compilation instructions
- Appendix C: Additional data or extended tests

### BIBLIOGRAPHY

- Follow FMFI BibTeX format
- Alexander Stanoyevitch – Discrete Structures with Contemporary Applications
- Academic papers on Merkle–Hellman, Shamir’s attack, improvements
- Any online sources used for algorithms or libraries



