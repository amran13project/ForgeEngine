from html.parser import HTMLParser
from pathlib import Path
from urllib.parse import unquote, urlsplit
import sys


class LinkCollector(HTMLParser):
    def __init__(self):
        super().__init__(convert_charrefs=True)
        self.links = []
        self.ids = set()

    def handle_starttag(self, tag, attrs):
        values = dict(attrs)
        if "id" in values:
            self.ids.add(values["id"])
        if tag == "a" and "href" in values:
            self.links.append(values["href"])
        if tag in {"img", "script", "link"}:
            attribute = "src" if tag in {"img", "script"} else "href"
            if attribute in values:
                self.links.append(values[attribute])


def main():
    site = Path("website")
    pages = sorted(site.rglob("*.html"))
    errors = []
    for page in pages:
        parser = LinkCollector()
        try:
            parser.feed(page.read_text(encoding="utf-8"))
            parser.close()
        except (OSError, UnicodeError, ValueError) as error:
            errors.append(f"{page}: cannot parse HTML: {error}")
            continue

        for link in parser.links:
            parts = urlsplit(link)
            if parts.scheme or parts.netloc or link.startswith("//"):
                continue
            if parts.path.startswith("/"):
                errors.append(f"{page}: root-relative URL breaks project-site paths: {link}")
                continue
            target = (page.parent / unquote(parts.path)).resolve() if parts.path else page.resolve()
            if target.is_dir():
                target = target / "index.html"
            try:
                target.relative_to(site.resolve())
            except ValueError:
                errors.append(f"{page}: local target escapes the website artifact: {link}")
                continue
            if not target.is_file():
                errors.append(f"{page}: missing local target: {link}")
            elif parts.fragment and target.suffix.lower() == ".html":
                target_parser = LinkCollector()
                try:
                    target_parser.feed(target.read_text(encoding="utf-8"))
                    target_parser.close()
                except (OSError, UnicodeError, ValueError) as error:
                    errors.append(f"{target}: cannot parse HTML: {error}")
                else:
                    if unquote(parts.fragment) not in target_parser.ids:
                        errors.append(f"{page}: missing fragment target: {link}")

    if errors:
        print("\n".join(errors), file=sys.stderr)
        return 1
    print(f"Website validation passed: {len(pages)} HTML pages, local links and fragments resolved.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
