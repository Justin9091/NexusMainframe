package models

type ListModulesResponse struct {
	Modules []Module
}

type ModuleActionResponse struct {
	Success bool
	Message string
}
