<script lang="ts">
	import { base } from '$app/paths';
	import { logs } from '$lib/logs';

	function resolveHref(path: string) {
		return `${base}${path}`;
	}
</script>

<section class="hero panel">
	<h1>Classical Knapsack Cryptosystem and its Variations</h1>
	<div class="lead">
		<p>
			This project studies the classical Merkle-Hellman knapsack cryptosystem, its known
			weaknesses, and selected improved variants, with a focus on demonstrating how specific
			parameter choices make the classical scheme breakable while contrasting variants and their
			computational complexity.
		</p>

		<h3>Project goals</h3>
		<ol>
			<li>Explain the classical scheme (key generation, encryption, decryption).</li>
			<li>Analyze cryptanalytic weaknesses.</li>
			<li>Compare selected variations by security and implementation complexity.</li>
		</ol>

		<h3>Current status</h3>
		<p>
			Scope refined after supervisor feedback, Makefile and repo cleaned up, and the site prepared
			for a live seminar walkthrough while I get ready to show the parameter-based break demo.
		</p>
	</div>

	<div class="hero-actions">
		<a class="button" href={resolveHref('/log/')}>View Weekly Log</a>
		<a class="button button-secondary" href={resolveHref('/topic/')}>Open Topic Notes</a>
	</div>
</section>

<section class="panel">
	<h2>Latest log entries</h2>
	<ul class="post-list">
		{#each logs as entry}
			<li>
				<div>
					<a href={resolveHref(`/log/${entry.slug}/`)}>{entry.title}</a>
					<p class="list-summary">{entry.summary}</p>
				</div>
				<span class="meta">{entry.date.split('-').reverse().join('.')}</span>
			</li>
		{/each}
	</ul>
</section>
