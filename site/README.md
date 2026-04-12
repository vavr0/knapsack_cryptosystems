# Seminar Site (SvelteKit)

This folder contains the SvelteKit version of the seminar website.

## Local development

```sh
bun install
bun run dev
```

## Typecheck

```sh
bun run check
```

## Production build

```sh
bun run build
```

The static output is generated in `site/build/`.

## GitHub Pages

This site is built with `@sveltejs/adapter-static` and deployed by `.github/workflows/svelte-pages.yml`.

Because the repository is published as a project page, production links use the `/knapsack_cryptosystems` base path.

## Content structure

- `src/routes/+page.svelte`: home page
- `src/routes/topic/+page.svelte`: topic notes
- `src/routes/resources/+page.svelte`: bibliography/resources
- `src/routes/log/+page.svelte`: weekly log index
- `src/routes/log/[slug]/+page.svelte`: weekly log detail page
- `src/lib/logs.ts`: weekly log data
- `static/`: PDFs and other static files
