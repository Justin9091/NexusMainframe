package models

type ListModulesRequest struct{}

type EnableModuleRequest struct {
	Name string
}

type DisableModuleRequest struct {
	Name string
}
