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
