<script lang="ts">
    import "../app.css";

    import { base } from "$app/paths";
    import { page } from "$app/state";
    import favicon from "$lib/assets/favicon.svg";

    let { children } = $props();

    const menuItems = [
        { label: "Home", href: "/" },
        { label: "Topic", href: "/topic/" },
        { label: "Log", href: "/log/" },
        { label: "Resources", href: "/resources/" },
    ];

    function resolveHref(path: string) {
        return `${base}${path}`;
    }

    function isCurrent(path: string) {
        const href = resolveHref(path);

        if (path === "/") {
            return page.url.pathname === href;
        }

        return page.url.pathname === href || page.url.pathname.startsWith(href);
    }
</script>

<svelte:head>
    <link rel="icon" href={favicon} />
</svelte:head>

<header class="site-header">
    <div class="container header-inner">
        <a class="brand" href={resolveHref("/")}
            >Bachelor Seminar Progress Log</a
        >
        <nav aria-label="Main navigation">
            <ul class="menu">
                {#each menuItems as item}
                    <li>
                        <a
                            href={resolveHref(item.href)}
                            aria-current={isCurrent(item.href)
                                ? "page"
                                : undefined}
                        >
                            {item.label}
                        </a>
                    </li>
                {/each}
            </ul>
        </nav>
    </div>
</header>

<main class="container">
    {@render children()}
</main>

<footer class="site-footer">
    <div class="container">
        <p class="footer-links">
            <a
                href="https://github.com/vavr0/knapsack_cryptosystems"
                target="_blank"
                rel="noopener noreferrer"
            >
                GitHub repository
            </a>
            <span aria-hidden="true">·</span>
            <a
                href={resolveHref("/knapsack_presentation.pdf")}
                target="_blank"
                rel="noopener noreferrer"
            >
                Presentation
            </a>
            <span aria-hidden="true">·</span>
            <a
                href={resolveHref("/zadanie-en.pdf")}
                target="_blank"
                rel="noopener noreferrer"
            >
                Thesis assignment
            </a>
        </p>
    </div>
</footer>
