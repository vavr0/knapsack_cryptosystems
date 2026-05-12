export type LogEntry = {
	slug: string;
	title: string;
	date: string;
	summary: string;
	completed: string[];
	nextSteps: string[];
};

export const logs: LogEntry[] = [
	{
		slug: 'week-13',
		title: 'Week 13 - Experiments and Results Draft',
		date: '2026-05-12',
		summary:
			'Ran the main experiment scripts, generated benchmark plots, and expanded the thesis results and implementation chapters.',
		completed: [
			'Added experiment scripts, requirements, CSV outputs, and plots for cryptosystem timing, attack timing, MITM table size, and parameter sweeps.',
			'Refined the benchmark and attack code so experiments can report the values needed by the thesis.',
			'Continued the thesis write-up by expanding the implementation, variations, and results sections and removing draft comments.'
		],
		nextSteps: [
			'Select the final plots and explain the important trends in the results chapter.',
			'Check that all experiment commands are reproducible from a clean checkout.',
			'Do a final pass over thesis wording, references, and figure captions.'
		]
	},
	{
		slug: 'week-12',
		title: 'Week 12 - Attack Modes and Output Cleanup',
		date: '2026-05-10',
		summary:
			'Wired attack selection into the CLI, added brute-force and meet-in-the-middle attack work, and separated output helpers from the main app flow.',
		completed: [
			'Added attack mode parsing and connected the iterated Merkle-Hellman scheme to the scheme selection path.',
			'Implemented and prepared brute-force and meet-in-the-middle attack paths for experiments.',
			'Moved printing and output helpers into a dedicated module and cleaned up benchmark/report formatting.'
		],
		nextSteps: [
			'Run consistent attack benchmarks for the schemes and parameter choices.',
			'Use the measured results to support the attack discussion in the thesis.',
			'Keep the CLI simple enough for the final demo.'
		]
	},
	{
		slug: 'week-11',
		title: 'Week 11 - Iterated Variant and Thesis Drafting',
		date: '2026-05-03',
		summary:
			'Added the base iterated scheme, shared common Merkle-Hellman helpers, and continued the written discussion of LLL and variants.',
		completed: [
			'Refactored duplicated classical and permuted scheme logic into common helper code.',
			'Added an initial iterated Merkle-Hellman implementation and updated scheme-related CLI behavior.',
			'Expanded thesis notes around variants, LLL-style attacks, and bibliography entries.'
		],
		nextSteps: [
			'Finish connecting the iterated variant to all demo and benchmark paths.',
			'Decide how much implementation detail belongs in the thesis versus the code appendix.',
			'Prepare attack code that can compare the classical and variant schemes.'
		]
	},
	{
		slug: 'week-10',
		title: 'Week 10 - Plaintext Input and Thesis Recovery',
		date: '2026-04-26',
		summary:
			'Implemented plaintext input support, refreshed the procedure presentation, and recovered thesis text on attacks and variations.',
		completed: [
			'Reworked the input buffer path and added plaintext parsing support to the CLI and app flow.',
			'Updated the seminar procedure slides and kept the website PDF in sync.',
			'Recovered and expanded thesis sections on implementation, theory, Shamir-style attacks, LLL, and variations.'
		],
		nextSteps: [
			'Finish output handling for plaintext demos.',
			'Connect the plaintext path to repeatable benchmark and attack examples.',
			'Continue turning recovered thesis material into polished prose.'
		]
	},
	{
		slug: 'week-09',
		title: 'Week 09 - Presentation and Repository Cleanup',
		date: '2026-04-19',
		summary:
			'Built the procedure presentation, embedded it into the site, cleaned tracked artifacts, and documented the code structure.',
		completed: [
			'Created and refreshed the procedure presentation PDF and added it to the website navigation.',
			'Added weekly log entries for the migrated Svelte site and removed old generated or temporary artifacts.',
			'Updated the main README and code README with build and project structure notes.'
		],
		nextSteps: [
			'Use the procedure slides during the next seminar walkthrough.',
			'Keep generated build outputs out of the repository.',
			'Return focus to plaintext input and the attack demonstration.'
		]
	},
	{
		slug: 'week-08',
		title: 'Week 08 - Stabilization After Variant Work',
		date: '2026-04-12',
		summary:
			'Did a small stabilization pass after the new variant work, including repository cleanup and minor app, benchmark, and seed handling fixes.',
		completed: [
			'Cleaned the ignore rules and removed noise from the repository state.',
			'Made small adjustments in the app and benchmark paths after the parameter and seed refactors.',
			'Checked the classical Merkle-Hellman code after the permuted variant was merged.'
		],
		nextSteps: [
			'Prepare the site and presentation for the next seminar update.',
			'Document the project layout so the implementation is easier to review.',
			'Plan the next concrete demo step around user-supplied plaintext.'
		]
	},
	{
		slug: 'week-07',
		title: 'Week 07 - New Variant and Research Notes',
		date: '2026-04-05',
		summary:
			'Finished the base parameter work, added a permuted Merkle-Hellman variant, and consolidated research notes for comparing schemes.',
		completed: [
			'Finished the base implementation changes around the updated Merkle-Hellman parameters.',
			'Added and registered the permuted Merkle-Hellman variant through the scheme resolver.',
			'Expanded research and summary notes to compare the classical and permuted variants.'
		],
		nextSteps: [
			'Test the new variant more systematically against the classical scheme.',
			'Turn research notes into thesis-ready text on variants and weaknesses.',
			'Decide which examples should appear in the final presentation and demo.'
		]
	},
	{
		slug: 'week-06',
		title: 'Week 06 - Randomness and Parameter Work',
		date: '2026-03-29',
		summary:
			'Cleaned repository artifacts, refactored seeding and randomness handling, and continued parameter work for the Merkle-Hellman implementation and benchmarks.',
		completed: [
			'Removed generated repository artifacts that should not stay tracked.',
			'Introduced seed and random support and wired it into the CLI, app, and benchmark paths.',
			'Improved benchmark inputs with random messages and cleaned up smaller helper and error-handling parts.'
		],
		nextSteps: [
			'Finalize the new parameter handling in the Merkle-Hellman implementation.',
			'Use the refactored randomness path in reproducible experiments.',
			'Document why the chosen parameters matter for attacks and demonstrations.'
		]
	},
	{
		slug: 'week-05',
		title: 'Week 05 - Resolver, Bench Fixes, and Seminar Materials',
		date: '2026-03-22',
		summary:
			'Finished the larger refactor, added scheme resolution support, stabilized benchmark timing, and updated seminar materials and website assets.',
		completed: [
			'Added the scheme resolver and completed the refactor across the app, bench, and scheme code.',
			'Repaired benchmark timing with warmup runs and more structured repetitions.',
			'Updated notes, refreshed presentation files, and polished related website parts.'
		],
		nextSteps: [
			'Implement more concrete scheme variants on top of the new resolver.',
			'Turn implementation notes into a clearer research summary.',
			'Keep the site and presentation aligned with the thesis direction.'
		]
	},
	{
		slug: 'week-04',
		title: 'Week 04 - Refactor and Execution Split',
		date: '2026-03-15',
		summary:
			'Refactored the execution pipeline, separated benchmarking from demo runs, and laid the groundwork for scheme-based abstractions.',
		completed: [
			'Refactored the runner and app pipeline and tightened CLI flag validation.',
			'Removed the old combined system path and separated benchmark and demo execution.',
			'Started the scheme abstraction layer and wrote down implementation notes for the next steps.'
		],
		nextSteps: [
			'Finish the scheme resolver and unify scheme-facing APIs.',
			'Stabilize benchmark timing after the refactor.',
			'Update project notes so the new architecture is easier to explain in the thesis.'
		]
	},
	{
		slug: 'week-03',
		title: 'Week 03 - Direction Update and Cleanup',
		date: '2026-03-08',
		summary:
			'Refactored the Makefile, cleaned up the code and site, and turned supervisor feedback into concrete thesis tasks.',
		completed: [
			'Refactored the Makefile to simplify build and run commands.',
			'Made small code and website cleanup updates.',
			'Reviewed supervisor feedback and translated it into concrete thesis tasks.'
		],
		nextSteps: [
			'Implement a parameter-based demonstration in code showing how the classical knapsack can be broken.',
			'Draft a section on knapsack variants, known attacks, and their weaknesses.',
			'Add explicit computational complexity notes to each method and attack.'
		]
	},
	{
		slug: '2026-03-01-week-02-seminar-prep',
		title: 'Week 02 - Seminar Prep',
		date: '2026-03-01',
		summary:
			'Prepared the seminar presentation, polished the site, fixed Pages routing issues, and improved weekly log readability.',
		completed: [
			'Prepared the seminar presentation for tomorrow.',
			'Polished seminar website content, structure, and navigation.',
			'Fixed GitHub Pages project-path routing and deployment issues.',
			'Improved weekly log formatting and readability.'
		],
		nextSteps: [
			'Implement a parameter-based demonstration in code showing how the classical knapsack can be broken.',
			'Draft a section on knapsack variants, known attacks, and their weaknesses.',
			'Add explicit complexity notes to each discussed method and attack.'
		]
	},
	{
		slug: '2026-02-22-week-01-api-refactor-research-setup',
		title: 'Week 01 - Setup',
		date: '2026-02-22',
		summary:
			'Cleaned up the API and module boundaries, refactored duplicated logic, and organized the initial cryptosystem references.',
		completed: [
			'Performed major API and codebase cleanup and refactor.',
			'Consolidated shared interfaces into common headers and aligned module boundaries.',
			'Refactored utility, system, bench, and CLI paths to improve maintainability.',
			'Organized initial references for knapsack cryptosystem background.'
		],
		nextSteps: [
			'Lock final API and module contracts after cleanup.',
			'Polish notes and narrative for seminar demo continuity.',
			'Prepare the Week 2 seminar-focused update.'
		]
	}
];

export const logsBySlug = Object.fromEntries(logs.map((entry) => [entry.slug, entry]));
