# Flatpak plugin [WIP]

### Status

#### Validated Dart API

```
getVersion
getDefaultArch
getSupportedArches
getSystemInstallations
getUserInstallation
getApplicationsInstalled
```

### Ubuntu Package Dependency

```
sudo apt install libflatpak-dev libxml2-dev zlib1g-dev
```

### Fedora Runtime Packages

```
sudo dnf install flatpak-devel libxml2-devel
```

### Example flatpak CLI usage

```
flatpak remotes
flatpak list
flatpak remote-add --if-not-exists flathub https://flathub.org/repo/flathub.flatpakrepo
flatpak remote-ls
flatpak install org.gnome.Todo
flatpak run org.gnome.Todo
```

### Generate message.g.h and messages.g.cc

    dart run pigeon --input pigeons/messages.dart

### Flatpak API reference

https://docs.flatpak.org/en/latest/libflatpak-api-reference.html
