workflow "Build" {
  on = "push"
  resolves = ["GitHub Action for Docker"]
}

action "GitHub Action for Docker" {
  uses = "docker://apiaryio/emcc"
  args = "-v $(pwd):/src"
  runs = "./docker/build.sh"
}
