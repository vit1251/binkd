
from invoke import task

@task
def docs(c):
    c.run("groff -Tascii -mman ./docs/binkd.8 | perl -npe 's/.\010//g' > binkd.txt", pty=True)

@task
def clean(c):
    c.run('rm -rf builddir', pty=True)

@task(default=True, pre=[clean])
def build(c):
    c.run('meson builddir', pty=True)
    c.run('ninja -C builddir', pty=True)
